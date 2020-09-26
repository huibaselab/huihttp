/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 *      Main function to entre the application.
 */


#include <hlog.h>
#include <huibase.h>
#include "hhapp.h"

using namespace HUIBASE;

int main(int argc, const char* argv[]) {

    Hhapp app(argc, argv);

    try {

        app.Init ();

        HBOOL cb = app.Run ();

        IF_FALSE(cb) {

            LOG_NS("Hhapp run error");
            return -1;

        }

    } catch (HCException& ex) {

        SLOG_ERROR("http app get an exception [%s]", ex.what());

    }

    return 0;

}
