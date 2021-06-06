#include <QPainter>
#include <QPixmap>
#include <shape/shape.h>

#ifndef BITMAPSHAPE_H
#define BITMAPSHAPE_H


class BitmapShape : public Shape {
    public:
        BitmapShape(QImage &image);
        BitmapShape(const BitmapShape &orig);
        bool hitTest(QPointF global_coord, qreal tolerance) const override;
        bool hitTest(QRectF global_coord_rect) const override;
        void calcBoundingBox() const override;
        void paint(QPainter *painter) const override;
        ShapePtr clone() const override;
        Shape::Type type() const override;
        QImage &image() const;
    private:
        QRectF bounding_rect_;
        unique_ptr<QPixmap> bitmap_;
        mutable QImage tinted_image_; //Cache object
        mutable std::uintptr_t tinted_signature; //Cache object
};

#endif // BITMAPSHAPE_H
