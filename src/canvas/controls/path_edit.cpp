#include <QApplication>
#include <QDebug>
#include <QPainterPath>
#include <canvas/controls/path_edit.h>
#include <cfloat>
#include <cmath>

using namespace Controls;

PathEdit::PathEdit(Scene &scene_) noexcept: CanvasControl(scene_) {
  dragging_index_ = -1;
  target_ = nullptr;
}

bool PathEdit::isActive() { return scene().mode() == Scene::Mode::PathEditing; }

bool PathEdit::mousePressEvent(QMouseEvent *e) {
  if (target_.get() == nullptr)
    return false;
  QPointF canvas_coord = scene().getCanvasCoord(e->pos());
  dragging_index_ = hitTest(getLocalCoord(canvas_coord));
  if (dragging_index_ > -1) {
    return true;
  } else {
    return false;
  }
}

bool PathEdit::mouseMoveEvent(QMouseEvent *e) {
  if (target_.get() == nullptr)
    return false;

  QPointF canvas_coord = scene().getCanvasCoord(e->pos());
  QPointF local_coord = getLocalCoord(canvas_coord);
  if (dragging_index_ > -1) {
    moveElementTo(dragging_index_, local_coord);

    if (is_closed_shape_) {
      if (dragging_index_ == 0 || dragging_index_ == path().elementCount() - 1) {
        moveElementTo(0, local_coord);
        moveElementTo(path().elementCount() - 1, local_coord);
      }
    }
  }
  return true;
}

void PathEdit::moveElementTo(int index, QPointF local_coord) {
  QPainterPath::Element ele = path().elementAt(index);
  QPointF offset = local_coord - ele;
  int last_index = path().elementCount() - 1;
  qInfo() << "Move element" << index;
  QPainterPath::Element prev_ele;
  QPainterPath::Element next_ele;
  QPainterPath::Element opposite_ele;
  QPainterPath::Element endpoint;
  PathShape::NodeType group_type;
  int opposite_index, endpoint_index;
  switch (cache_[index].type) {
    case PathShape::NodeType::CurveSymmetry:
    case PathShape::NodeType::CurveSmooth:
    case PathShape::NodeType::CurveCorner:
      prev_ele = path().elementAt(index - 1);
      path().setElementPositionAt(index - 1, prev_ele.x + offset.x(), prev_ele.y + offset.y());

      next_ele = path().elementAt((index + 1) % last_index);
      if (next_ele.type == QPainterPath::ElementType::CurveToElement) {
        path().setElementPositionAt((index + 1) % last_index, next_ele.x + offset.x(), next_ele.y + offset.y());
        target().parent()->flushCache();
      }
      break;
    case PathShape::NodeType::CurveCtrlPrev:
    case PathShape::NodeType::CurveCtrlNext:
      if (cache_[index].type == PathShape::NodeType::CurveCtrlPrev) {
        endpoint_index = index + 1;
        opposite_index = (index + 2) % last_index;
      } else {
        endpoint_index = is_closed_shape_ && index == 1 ? last_index : index - 1;
        opposite_index = index - 2 >= 0 ? index - 2 : last_index - 1;
      }

      group_type = cache_[endpoint_index].type;
      endpoint = path().elementAt(endpoint_index);
      opposite_ele = path().elementAt(opposite_index);
      if (opposite_ele.type == QPainterPath::ElementType::CurveToElement ||
          opposite_ele.type == QPainterPath::ElementType::CurveToDataElement) {
        if (group_type == PathShape::NodeType::CurveSymmetry) {
          path().setElementPositionAt(opposite_index, opposite_ele.x - offset.x(), opposite_ele.y - offset.y());
        } else if (group_type == PathShape::NodeType::CurveSmooth) {
          QPointF new_pos = endpoint + (endpoint - local_coord) * distance(opposite_ele - endpoint) /
                                       distance(endpoint - local_coord);
          path().setElementPositionAt(opposite_index, new_pos.x(), new_pos.y());
          target().parent()->flushCache();
        }
      }
      break;
    default:
      break;
  }
  path().setElementPositionAt(index, local_coord.x(), local_coord.y());
  target().flushCache();
}

qreal PathEdit::distance(QPointF point) { return sqrt(pow(point.x(), 2) + pow(point.y(), 2)); }

bool PathEdit::hoverEvent(QHoverEvent *e, Qt::CursorShape *cursor) {
  if (target_.get() == nullptr)
    return false;

  QPointF canvas_coord = scene().getCanvasCoord(e->pos());
  QPointF local_coord = getLocalCoord(canvas_coord);

  if (hitTest(local_coord) > -1) {
    *cursor = Qt::SizeAllCursor;
    return true;
  } else {
    return false;
  }
}

bool PathEdit::mouseReleaseEvent(QMouseEvent *e) {
  if (target_.get() == nullptr)
    return false;
  QPointF canvas_coord = scene().getCanvasCoord(e->pos());

  return true;
}

int PathEdit::hitTest(QPointF local_coord) {
  if (target_.get() == nullptr)
    return -1;
  float tolerance = 8 / scene().scale();
  for (int i = 0; i < path().elementCount(); i++) {
    QPainterPath::Element ele = path().elementAt(i);
    if ((ele - local_coord).manhattanLength() < tolerance) {
      return i;
    }
  }
  return -1;
}

void PathEdit::paint(QPainter *painter) {
  if (scene().mode() != Scene::Mode::PathEditing)
    return;
  if (target_.get() == nullptr)
    return;

  auto sky_blue = QColor::fromRgb(0x00, 0x99, 0xCC, 255);
  auto blue_pen = QPen(sky_blue, 2, Qt::SolidLine);
  auto blue_thin_pen = QPen(sky_blue, 1, Qt::SolidLine);
  auto blue_large_pen = QPen(sky_blue, 10, Qt::SolidLine);
  auto blue_small_pen = QPen(sky_blue, 6, Qt::SolidLine);
  blue_pen.setCosmetic(true);
  blue_thin_pen.setCosmetic(true);
  blue_large_pen.setCosmetic(true);
  blue_small_pen.setCosmetic(true);

  painter->setPen(blue_pen);
  QTransform transform = target_->transform();
  QPolygonF lines;
  QPolygonF large_points;
  QPolygonF small_points;
  for (int i = 0; i < path().elementCount(); i++) {
    QPainterPath::Element ele = path().elementAt(i);
    if (ele.isMoveTo()) {
      large_points << ele;
    } else if (ele.isLineTo()) {
      large_points << ele;
    } else if (ele.isCurveTo()) {
      large_points << path().elementAt(i + 2);
      small_points << ele;
      small_points << path().elementAt(i + 1);
      lines << path().elementAt(i + 1);
      lines << path().elementAt(i + 2);
      lines << ele;
      lines << path().elementAt(i - 1);
    }
  }
  large_points = transform.map(large_points);
  small_points = transform.map(small_points);
  QVector<QRectF> large_rects;
  QVector<QRectF> small_rects;
  float large_size = 4 / scene().scale();
  float small_size = 2 / scene().scale();
  for (auto &p : large_points) {
    large_rects << QRectF(p.x() - large_size, p.y() - large_size, large_size * 2, large_size * 2);
  }
  for (auto &p : small_points) {
    small_rects << QRectF(p.x() - small_size, p.y() - small_size, small_size * 2, small_size * 2);
  }
  painter->setPen(blue_pen);
  painter->drawRects(large_rects);
  painter->drawRects(small_rects);

  painter->save();

  painter->setTransform(transform, true);
  painter->drawLines(lines);

  painter->setPen(blue_thin_pen);
  painter->drawPath(path());

  painter->restore();
}

void PathEdit::reset() { target_ = nullptr; }

QPainterPath &PathEdit::path() { return target().path_; }

QPointF PathEdit::getLocalCoord(QPointF canvas_coord) { return target_->transform().inverted().map(canvas_coord); }

PathShape &PathEdit::target() { return *dynamic_cast<PathShape *>(target_.get()); }

void PathEdit::setTarget(ShapePtr &target) {
  target_ = target;
  int elem_count = path().elementCount();
  is_closed_shape_ =
       elem_count > 0 && (path().elementAt(0) - path().elementAt(elem_count - 1)).manhattanLength() <= FLT_EPSILON;
  cache_.clear();

  for (int i = 0; i < path().elementCount(); i++) {
    QPainterPath::Element ele = path().elementAt(i);
    if (ele.isMoveTo()) {
      cache_ << PathNode(PathShape::NodeType::MOVE_TO);
    } else if (ele.isLineTo()) {
      cache_ << PathNode(PathShape::NodeType::LINE_TO);
    } else if (ele.isCurveTo()) {
      cache_ << PathNode(PathShape::NodeType::CurveCtrlNext);
      cache_ << PathNode(PathShape::NodeType::CurveCtrlPrev);
      cache_ << PathNode(PathShape::NodeType::CurveSmooth);
    }
  }
}

bool PathEdit::keyPressEvent(QKeyEvent *e) {
  if (e->key() == Qt::Key::Key_Escape) {
    endEditing();
  }
  return true;
}

void PathEdit::endEditing() {
  scene().setMode(Scene::Mode::Selecting);
  scene().clearSelections();
  target_->flushCache();
  scene().setSelection(target_);
  reset();
}