#include <nodes/GraphModel>
#include <nodes/GraphicsScene>
#include <nodes/GraphicsView>
#include <nodes/ConnectionStyle>

#include <QtWidgets/QApplication>


static
void
setStyle()
{
  ConnectionStyle::setConnectionStyle(
    R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}


int
main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  setStyle();

  GraphModel graphModel;

  auto scene = new GraphicsScene(graphModel);

  GraphicsView view(scene);


  view.setWindowTitle("Simplest model-based graph");
  view.resize(800, 600);
  view.showNormal();

  return app.exec();
}
