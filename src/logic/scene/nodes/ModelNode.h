#pragma once
#include "NodeBase.h"
#include <QColor>
#include <QVector>
#include <QVector3D>

class ModelNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)
    Q_PROPERTY(QString modelRole READ modelRole WRITE setModelRole)
    Q_PROPERTY(QString parentTransformId READ parentTransformId WRITE setParentTransformId)
    Q_PROPERTY(bool showEdgesFlag READ isShowEdges WRITE setShowEdges)
    Q_PROPERTY(bool backfaceCullingFlag READ isBackfaceCulling WRITE setBackfaceCulling)
    Q_PROPERTY(bool useScalarColorFlag READ isUseScalarColor WRITE setUseScalarColor)

public:
    explicit ModelNode(QObject* parent = nullptr);

    // --- PolyData / mesh ---
    void setPolyData(const QByteArray& data);
    QByteArray getPolyData() const;
    void clearPolyData();
    void setMeshData(const QVector<QVector3D>& vertices, const QVector<int>& indices);
    QVector<QVector3D> getVertices() const;
    QVector<int> getIndices() const;
    QVector<double> getBoundingBox() const;

    // --- File path (convenience) ---
    QString filePath() const;
    void setFilePath(const QString& path);

    // --- Visualization (per design conventions) ---
    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;
    double opacity() const;
    void setOpacity(double opacity);
    double getOpacity() const;
    bool isVisible() const;
    void setVisibility(bool visible);
    bool wireframe() const;
    void setWireframe(bool wf);
    QString renderMode() const;
    void setRenderMode(const QString& mode);
    QString getRenderMode() const;

    // --- Model metadata ---
    QString modelRole() const;
    void setModelRole(const QString& role);
    QString parentTransformId() const;
    void setParentTransformId(const QString& id);
    QString getParentTransform() const;
    void setParentTransform(const QString& id);

    // --- Edge display ---
    bool isShowEdges() const;
    void setShowEdges(bool show);
    QColor getEdgeColor() const;
    void setEdgeColor(const QColor& color);
    double getEdgeWidth() const;
    void setEdgeWidth(double width);

    // --- Backface culling ---
    bool isBackfaceCulling() const;
    void setBackfaceCulling(bool cull);

    // --- Scalar color ---
    bool isUseScalarColor() const;
    void setUseScalarColor(bool use);
    QString getScalarColorMap() const;
    void setScalarColorMap(const QString& mapName);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QByteArray polyDataPayload_;
    QVector<QVector3D> vertices_;
    QVector<int> indices_;
    QString filePath_;
    QColor colorValue_{Qt::white};
    double opacity_{1.0};
    bool visibilityFlag_{true};
    bool wireframe_{false};
    QString renderMode_{"surface"};
    QString modelRole_;
    QVector<double> cachedBounds_;   // [xmin, xmax, ymin, ymax, zmin, zmax]
    QString parentTransformId_;
    bool showEdgesFlag_{false};
    QColor edgeColorRGBA_{Qt::black};
    double edgeWidthValue_{1.0};
    bool backfaceCullingFlag_{false};
    bool useScalarColorFlag_{false};
    QString scalarColorMapName_;

    void updateCachedBounds();
};
