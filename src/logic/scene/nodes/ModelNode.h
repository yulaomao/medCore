// 文件说明：模型节点声明，负责保存三维模型数据及其可视化属性。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "NodeBase.h"
#include <QColor>
#include <QVector>
#include <QVector3D>

/// 模型节点。
///
/// 保存模型网格、显示模式、边显示和标量着色等与三维模型相关的信息。
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

    // --- PolyData / 网格数据 ---
    void setPolyData(const QByteArray& data);
    QByteArray getPolyData() const;
    void clearPolyData();
    void setMeshData(const QVector<QVector3D>& vertices, const QVector<int>& indices);
    QVector<QVector3D> getVertices() const;
    QVector<int> getIndices() const;
    QVector<double> getBoundingBox() const;

    // --- 文件路径便捷访问 ---
    QString filePath() const;
    void setFilePath(const QString& path);

    // --- 可视化属性 ---
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

    // --- 模型元数据 ---
    QString modelRole() const;
    void setModelRole(const QString& role);
    QString parentTransformId() const;
    void setParentTransformId(const QString& id);
    QString getParentTransform() const;
    void setParentTransform(const QString& id);

    // --- 边缘显示 ---
    bool isShowEdges() const;
    void setShowEdges(bool show);
    QColor getEdgeColor() const;
    void setEdgeColor(const QColor& color);
    double getEdgeWidth() const;
    void setEdgeWidth(double width);

    // --- 背面剔除 ---
    bool isBackfaceCulling() const;
    void setBackfaceCulling(bool cull);

    // --- 标量着色 ---
    bool isUseScalarColor() const;
    void setUseScalarColor(bool use);
    QString getScalarColorMap() const;
    void setScalarColorMap(const QString& mapName);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QByteArray polyDataPayload_;            // 原始 PolyData 二进制负载。
    QVector<QVector3D> vertices_;           // 顶点数组。
    QVector<int> indices_;                  // 索引数组。
    QString filePath_;                      // 模型文件路径。
    QColor colorValue_{Qt::white};          // 模型主颜色。
    double opacity_{1.0};                   // 透明度。
    bool visibilityFlag_{true};             // 是否可见。
    bool wireframe_{false};                 // 是否以线框模式输出。
    QString renderMode_{"surface"};        // 具体渲染模式。
    QString modelRole_;                     // 模型业务角色。
    QVector<double> cachedBounds_;          // 缓存包围盒：[xmin, xmax, ymin, ymax, zmin, zmax]。
    QString parentTransformId_;             // 父变换节点标识。
    bool showEdgesFlag_{false};             // 是否显示边缘。
    QColor edgeColorRGBA_{Qt::black};       // 边缘颜色。
    double edgeWidthValue_{1.0};            // 边缘线宽。
    bool backfaceCullingFlag_{false};       // 是否开启背面剔除。
    bool useScalarColorFlag_{false};        // 是否使用标量着色。
    QString scalarColorMapName_;            // 标量颜色映射名称。

    void updateCachedBounds();
};
