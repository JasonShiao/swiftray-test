#ifndef VCANVAS_H
#define VCANVAS_H
#include <QtQuick>
#include <shape/shape.hpp>
#include <parser/svgpp_parser.hpp>
#include <canvas/scene.hpp>
#include <canvas/transform_box.hpp>
#include <canvas/rect_drawer.h>

class VCanvas : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

    public:
        VCanvas(QQuickItem *parent = 0);
        void paint(QPainter *painter) override;
        void loop();
        void loadSVG(QByteArray &data);
        void keyPressEvent(QKeyEvent *e) override;
        void mousePressEvent(QMouseEvent *e) override;
        void mouseMoveEvent(QMouseEvent *e) override;
        void mouseReleaseEvent(QMouseEvent *e) override;
        void wheelEvent(QWheelEvent *e) override;
        bool event(QEvent *e) override;
        void removeSelection();
        Scene &scene();
        Scene *scenePtr();

    public Q_SLOTS:
        void editCut();
        void editCopy();
        void editPaste();
        void editDelete();
        void editUndo();
        void editRedo();
        void editSelectAll();
        void editGroup();
        void editUngroup();

    private:
        bool ready;
        int counter;
        Scene scene_;
        SVGPPParser svgpp_parser;
        TransformBox transform_box;
        RectDrawer rect_drawer;

        QTimer *timer;
        bool small_screen;
        QPoint mouse_press;
        bool mouse_drag;
        QRectF selection_box;
        QPointF selection_start;
        QHash<int, int> m_fingerPointMapping;


    signals:
        void rightAlignedChanged();
};

#endif // VCANVAS_H
