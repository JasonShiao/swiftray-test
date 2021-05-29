#include <QPainter>
#include <QPainterPath>

#ifndef SHAPE_H
#define SHAPE_H

using namespace std;
// Use like struct first (can run faster..)
class Shape {
    public:
        Shape() noexcept;
        virtual ~Shape();
        // Common members
        bool selected;
        QPointF pos() const;
        qreal x() const;
        qreal y() const;
        qreal rotation() const;
        qreal scaleX() const;
        qreal scaleY() const;
        qreal setX(qreal x);
        qreal setY(qreal y);
        void setPos(QPointF pos);
        qreal setRotation(qreal r);
        QTransform transform() const;
        void applyTransform(QTransform transform);
        void setTransform(QTransform transform);

        // Virtual functions
        virtual QRectF boundingRect() const;
        virtual void cacheSelectionTestingData() ;
        virtual bool testHit(QPointF global_coord, qreal tolerance) const;
        virtual bool testHit(QRectF global_coord_rect) const;
        virtual void simplify();
        virtual void paint(QPainter *painter) const;
        virtual shared_ptr<Shape> clone() const;

    private:
        QTransform transform_;
        int ref_count_;
};

typedef shared_ptr<Shape> ShapePtr;

#endif //SHAPE_H