#ifndef COMMAND_H
#define COMMAND_H

#include <layer.h>
#include <shape/shape.h>
#include <shape/text-shape.h>

// TODO add mode change command if we need to do redo in certain modes..
// TODO simplify command usage
// TODO create batch command for add / remove shapes
class Document;
// TODO (Move implementation to command.cpp)
namespace Commands {
  class JoinedCmd;

  class BaseCmd {
  public:

    virtual void undo(Document *doc) {
      Q_ASSERT_X(false, "Commands", "This command did not implement undo");
    }

    virtual void redo(Document *doc) {
      Q_ASSERT_X(false, "Commands", "This command did not implement redo");
    }

    shared_ptr<JoinedCmd> operator+(BaseCmd *another_event);
  };

  typedef shared_ptr<BaseCmd> CmdPtr;

  class AddLayerCmd : public BaseCmd {
  public:
    AddLayerCmd(const LayerPtr &layer) : layer_(layer) {}

    void undo(Document *doc) override;

    void redo(Document *doc) override;

    LayerPtr layer_;
  };

  class RemoveLayerCmd : public BaseCmd {
  public:
    RemoveLayerCmd(const LayerPtr &layer) : layer_(layer) {}

    void undo(Document *doc) override;

    void redo(Document *doc) override;

    LayerPtr layer_;
  };

  class AddShapeCmd : public BaseCmd {
  public:
    AddShapeCmd(Layer *layer, const ShapePtr &shape) :
         layer_(layer), shape_(shape) {}

    void undo(Document *doc) override;

    void redo(Document *doc) override;

    // Shape events don't need to manage layer's lifecycle
    Layer *layer_;
    ShapePtr shape_;
  };

  class RemoveShapeCmd : public BaseCmd {
  public:
    explicit RemoveShapeCmd(const ShapePtr &shape) :
         shape_(shape) { layer_ = shape->layer(); }

    RemoveShapeCmd(Layer *layer, const ShapePtr &shape) :
         layer_(layer), shape_(shape) {}

    void undo(Document *doc) override;

    void redo(Document *doc) override;

    // Shape events don't need to manage layer's lifecycle
    Layer *layer_;
    ShapePtr shape_;
  };

  class SelectCmd : public BaseCmd {
  public:

    explicit SelectCmd(Document *doc, const QList<ShapePtr> &new_selections_);

    void undo(Document *doc) override;

    void redo(Document *doc) override;

    QList<ShapePtr> old_selections_;
    QList<ShapePtr> new_selections_;
  };

  class JoinedCmd : public BaseCmd {
  public:

    JoinedCmd() {}

    // Constructor for joining multiple events
    JoinedCmd(initializer_list<BaseCmd *> undo_events) {
      for (auto &event : undo_events)
        events << CmdPtr(event);
    }

    // Constructor for joining multiple events (event ptr)
    JoinedCmd(initializer_list<CmdPtr> undo_events) {
      for (auto &event : undo_events)
        events << event;
    }

    void undo(Document *doc) override {
      for (int i = events.size() - 1; i >= 0; i--) {
        events.at(i)->undo(doc);
      }
    }

    void redo(Document *doc) override {
      for (auto &event : events) {
        event->redo(doc);
      }
    }

    QList<CmdPtr> events;
  };

  typedef shared_ptr<JoinedCmd> JoinedPtr;

  // Command when object's property is changed, and the property can be "passed by value"
  template<typename T, typename PropType, PropType (T::*PropGetter)() const, void (T::*PropSetter)(
       PropType)>
  class SetCmd : public BaseCmd {
  public:

    explicit SetCmd(T *target, PropType new_value) : target_(target), new_value_(new_value) {
      old_value_ = (target_->*PropGetter)();
    }

    void undo(Document *doc) override {
      qInfo() << "[Command] Undo set";
      (target_->*PropSetter)(old_value_);
    }

    void redo(Document *doc) override {
      qInfo() << "[Command] Do set";
      (target_->*PropSetter)(new_value_);
    };

    T *target_;
    PropType new_value_;
    PropType old_value_;
  };

  // Command when object's property is changed, and the property is usually "passed by reference"
  template<typename T, typename PropType, const PropType &(T::*PropGetter)() const, void (T::*PropSetter)(
       const PropType &)>
  class SetRefCmd : public BaseCmd {
  public:

    explicit SetRefCmd(T *target, PropType new_value) : target_(target), new_value_(new_value) {
      old_value_ = (target_->*PropGetter)();
    }

    void undo(Document *doc) override {
      qInfo() << "[Command] Undo setRef";
      (target_->*PropSetter)(old_value_);
    }

    void redo(Document *doc) override {
      qInfo() << "[Command] Do setRef";
      (target_->*PropSetter)(new_value_);
    };

    T *target_;
    PropType new_value_;
    PropType old_value_;
  };

  // Operators overload
  JoinedPtr operator+(const CmdPtr &a, const CmdPtr &b);

  JoinedPtr &operator<<(JoinedPtr &a, const CmdPtr &b);

  JoinedPtr &operator<<(JoinedPtr &a, BaseCmd *b);

  // Abbreviations for generating commands
  template<typename T, typename PropType, PropType (T::*PropGetter)() const, void (T::*PropSetter)(
       PropType)>
  CmdPtr Set(T *target, PropType new_value) {
    return make_shared<SetCmd<T, PropType, PropGetter, PropSetter>>(target, new_value);
  }

  template<typename T, typename PropType, const PropType &(T::*PropGetter)() const, void (T::*PropSetter)(
       const PropType &)>
  CmdPtr SetRef(T *target, PropType new_value) {
    return make_shared<SetRefCmd<T, PropType, PropGetter, PropSetter>>
         (target, new_value);
  }

  // Abbreviations for specific set
  constexpr CmdPtr
  (*SetTransform)(Shape *, QTransform) =
  &SetRef<Shape, QTransform, &Shape::transform, &Shape::setTransform>;

  constexpr CmdPtr (*SetParent)(Shape *, Shape *) =
  &Set<Shape, Shape *, &Shape::parent, &Shape::setParent>;

  constexpr CmdPtr (*SetLayer)(Shape *, Layer *) =
  &Set<Shape, Layer *, &Shape::layer, &Shape::setLayer>;

  constexpr CmdPtr (*SetFont)(TextShape *, QFont) =
  &SetRef<TextShape, QFont, &TextShape::font, &TextShape::setFont>;

  constexpr CmdPtr (*SetLineHeight)(TextShape *, float) =
  &Set<TextShape, float, &TextShape::lineHeight, &TextShape::setLineHeight>;

  constexpr CmdPtr (*SetRotation)(Shape *, qreal) =
  &Set<Shape, qreal, &TextShape::rotation, &TextShape::setRotation>;

  CmdPtr AddShape(Layer *layer, const ShapePtr &shape);

  CmdPtr RemoveShape(const ShapePtr &shape);

  CmdPtr RemoveShape(Layer *layer, const ShapePtr &shape);

  CmdPtr Select(Document *doc, const QList<ShapePtr> &new_selections);

  template<typename T, typename PropType, PropType (T::*PropGetter)() const, void (T::*PropSetter)(
       PropType)>
  CmdPtr Set(T *target, PropType new_value);

  template<typename T, typename PropType, const PropType &(T::*PropGetter)() const, void (T::*PropSetter)(
       const PropType &)>
  CmdPtr SetRef(T *target, PropType new_value);

  CmdPtr AddShapes(Layer *layer, const QList<ShapePtr> &shapes);

  CmdPtr RemoveShapes(const QList<ShapePtr> &shapes);

  CmdPtr AddLayer(const LayerPtr &layer);

  CmdPtr RemoveLayer(const LayerPtr &layer);

  CmdPtr RemoveSelections(Document *doc);

  JoinedPtr Joined();
}

typedef Commands::CmdPtr CmdPtr;
typedef Commands::BaseCmd BaseCmd;

#endif