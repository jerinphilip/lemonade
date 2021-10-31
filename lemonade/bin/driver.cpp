#include <QCommandLineParser>
#include <QStandardPaths>
#include <iostream>

int main(int argc, char **argv) {
  QCoreApplication(argc, argv);
  QCoreApplication::setApplicationName("lemonade");

#define qdisplay(x)                                                            \
  do {                                                                         \
    auto located = QStandardPaths::writableLocation(x);                        \
    if (located.isEmpty()) {                                                   \
      std::cerr << "Unable to find " << #x << std::endl;                       \
    }                                                                          \
    std::cout << #x << ": " << located.toStdString() << std::endl;             \
    std::cout << #x << ": " << QStandardPaths::displayName(x).toStdString()    \
              << std::endl;                                                    \
  } while (0)

  qdisplay(QStandardPaths::DataLocation);
  qdisplay(QStandardPaths::AppDataLocation);
  qdisplay(QStandardPaths::ConfigLocation);
  qdisplay(QStandardPaths::AppConfigLocation);
  qdisplay(QStandardPaths::CacheLocation);

  auto modelJSON_ =
      QStandardPaths::locate(QStandardPaths::AppConfigLocation, "models.json");
  std::cout << modelJSON_.toStdString() << std::endl;

  auto modelDir_ =
      QStandardPaths::locate(QStandardPaths::AppDataLocation, "models",
                             QStandardPaths::LocateDirectory);
  std::cout << modelDir_.toStdString() << std::endl;
#undef qdisplay
  return 0;
}
