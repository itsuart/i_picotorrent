#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <memory>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/database.hpp>
#include <picotorrent/core/environment.hpp>
#include <picotorrent/geoip/geoip.hpp>

#include "application.hpp"
#include "mainwindow.hpp"
#include "translator.hpp"

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    pt::Application app(argc, argv);

    if (!app.isSingleInstance())
    {
        app.activateOtherInstance();
        return 0;
    }

    auto env = std::make_shared<pt::Environment>();
    auto db = std::make_shared<pt::Database>(env);

    if (!db->migrate())
    {
        return -1;
    }

    auto cfg = std::make_shared<pt::Configuration>(db);

    pt::Translator& translator = pt::Translator::instance();
    translator.loadEmbedded(GetModuleHandle(NULL));
    translator.setLanguage(cfg->getInt("language_id"));

    pt::MainWindow wnd(env, db, cfg);

    switch (cfg->getInt("start_position"))
    {
    case pt::Configuration::WindowState::Hidden: // sys tray
        // If we don't show PicoTorrent in the notification area, show it
        // normally.
        if (!cfg->getBool("show_in_notification_area"))
        {
            wnd.showNormal();
        }
        break;
    case pt::Configuration::WindowState::Maximized:
        wnd.showMaximized();
        break;
    case pt::Configuration::WindowState::Minimized:
        wnd.showMinimized();
        break;

    case pt::Configuration::WindowState::Normal:
    default:
        wnd.showNormal();
        break;
    }

    wnd.handleCommandLine(app.arguments());

    return app.exec();
}
