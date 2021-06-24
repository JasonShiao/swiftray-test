#ifndef VCANVAS_H
#define VCANVAS_H

#include <QtQuick>
#include <canvas/controls/canvas-control.h>
#include <canvas/controls/grid.h>
#include <canvas/controls/line.h>
#include <canvas/controls/select.h>
#include <canvas/controls/oval.h>
#include <canvas/controls/path-draw.h>
#include <canvas/controls/path-edit.h>
#include <canvas/controls/rect.h>
#include <canvas/controls/text.h>
#include <canvas/controls/transform.h>
#include <document.h>
#include <parser/svgpp-parser.h>
#include <shape/shape.h>
#include <canvas/memory-monitor.h>
#include <clipboard.h>
#include <gcode/generators/preview-generator.h>

/*The canvas should be designed to handle multiple documents,
  carefully choose what properties you want to put in the canvas,
  and what properties you want to put in the document. */
class Canvas : public QQuickPaintedItem {
Q_OBJECT
  QML_ELEMENT

public:
  enum class Mode {
    Selecting,
    Moving,
    MultiSelecting,
    Transforming,
    Rotating,
    RectDrawing,
    LineDrawing,
    OvalDrawing,
    PathDrawing,
    PathEditing,
    TextDrawing
  };

  Canvas(QQuickItem *parent = 0);

  ~Canvas();

  void paint(QPainter *painter) override;

  void loop();

  void loadSVG(QByteArray &data);

  void keyPressEvent(QKeyEvent *e) override;

  void mousePressEvent(QMouseEvent *e) override;

  void mouseMoveEvent(QMouseEvent *e) override;

  void mouseReleaseEvent(QMouseEvent *e) override;

  void mouseDoubleClickEvent(QMouseEvent *e) override;

  void wheelEvent(QWheelEvent *e) override;

  bool event(QEvent *e) override;

  Document &document();

  Controls::Transform &transformControl() { return ctrl_transform_; }

  Clipboard &clipboard();

  Mode mode() const;

  const QFont &font() const;

  // Graphics should be drawn in lower quality is this return true
  bool isVolatile() const;

  // Setters
  void setDocument(Document *document);

  void setMode(Mode mode);

public slots:

  void editCut();

  void editCopy();

  void editPaste();

  void editDelete();

  void editUndo();

  void editRedo();

  void editSelectAll();

  void editGroup();

  void editUngroup();

  void editDrawRect();

  void editDrawOval();

  void editDrawLine();

  void editDrawPath();

  void editDrawText();

  void editUnion();

  void editSubtract();

  void editIntersect();

  void editDifference();

  void addEmptyLayer();

  void importImage(QImage &image);

  void setActiveLayer(LayerPtr &layer);

  void setLayerOrder(QList<LayerPtr> &order);

  void fitToWindow();

  void setFont(const QFont &font);

  void emitAllChanges();

  shared_ptr<PreviewGenerator> exportGcode();

  void setWidgetSize(QSize widget_size);

  void setWidgetOffset(QPoint offset);

  void setLineHeight(float line_height);

  void backToSelectMode();

  void startMemoryMonitor();

private:
  // Basic attributes
  unique_ptr<Document> doc_;
  Mode mode_;
  QFont font_;
  Clipboard clipboard_;
  SVGPPParser svgpp_parser_;

  // Control components
  Controls::Transform ctrl_transform_;
  Controls::Select ctrl_select_;
  Controls::Grid ctrl_grid_;
  Controls::Line ctrl_line_;
  Controls::Oval ctrl_oval_;
  Controls::PathDraw ctrl_path_draw_;
  Controls::PathEdit ctrl_path_edit_;
  Controls::Rect ctrl_rect_;
  Controls::Text ctrl_text_;
  QList<Controls::CanvasControl *> ctrls_;


  // Display attributes
  QPoint widget_offset_;
  QElapsedTimer volatility_timer;

  // Monitor attributes
  QElapsedTimer fps_timer;
  int fps_count;
  float fps;
  QTimer *timer;
  QThread *mem_thread_;
  MemoryMonitor mem_monitor_;

signals:

  void selectionsChanged();

  void layerChanged();

  void modeChanged();

  void undoCalled();

  void redoCalled();

  void transformChanged(qreal x, qreal y, qreal r, qreal w, qreal h);
};

#endif // VCANVAS_H