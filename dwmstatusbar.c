/*
Very simple statusbar for dwm (dynamic window manager):
battery capacity, link status, date-time.
Thanks to everyone for their help and code examples.
Andrey Shashanov (2019)
Set your: IFACE, BATTERY
gcc -O2 -s -lX11 -o dwmstatusbar dwmstatusbar.c
*/

#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define IFACE "wlp3s0"
#define BATTERY "BAT0"

#define PATH_LINK "/sys/class/net/" IFACE "/operstate"
#define PATH_CAPACITY "/sys/class/power_supply/" BATTERY "/capacity"
#define DATETIME_FORMAT "  %Y-%m-%d %a %H:%M"
#define SLEEP_SEC 5
#define SLEEP_1 2  /* SLEEP_SEC * SLEEP_1 */
#define SLEEP_2 24 /* SLEEP_SEC * SLEEP_2 */

int main(void)
{
    Display *dpy;
    Window w;
    struct timespec ts;
    size_t count_1, count_2;

    /* setlocale(LC_TIME, "ru_RU.UTF-8"); */

    if ((dpy = XOpenDisplay(NULL)) == NULL)
        return EXIT_FAILURE;

    w = DefaultRootWindow(dpy);

    ts.tv_sec = SLEEP_SEC;
    ts.tv_nsec = 0;

    count_1 = 1;
    count_2 = 1;

    for (;; nanosleep(&ts, NULL))
    {
        char capacity[32], lnk[32], datetime[32],
            buf[32 + sizeof capacity + sizeof lnk + sizeof datetime];
        int fd;
        ssize_t n;
        time_t timer;
        struct tm *timeptr;

        if (!(--count_1))
        {
            count_1 = SLEEP_1;

            if ((fd = open(PATH_LINK, O_RDONLY)) != -1)
            {
                if ((n = read(fd, lnk, sizeof lnk)) > 0)
                    /* truncate newline */
                    lnk[--n] = '\0';
                else
                    lnk[0] = '\0';
                close(fd);
            }
            else
                lnk[0] = '\0';
        }

        if (!(--count_2))
        {
            count_2 = SLEEP_2;

            if ((fd = open(PATH_CAPACITY, O_RDONLY)) != -1)
            {
                if ((n = read(fd, capacity, sizeof capacity)) > 0)
                    /* truncate newline */
                    capacity[--n] = '\0';
                else
                    capacity[0] = '\0';
                close(fd);
            }
            else
                capacity[0] = '\0';
        }

        timer = time(NULL);
        timeptr = localtime(&timer);
        strftime(datetime, sizeof datetime, DATETIME_FORMAT, timeptr);

        strcpy(buf, " " BATTERY ":");
        strcat(buf, capacity);
        strcat(buf, "%  " IFACE ":");
        strcat(buf, lnk);
        strcat(buf, datetime);

        XStoreName(dpy, w, buf);
        XFlush(dpy);
    }

    /* code will never be executed */
    XCloseDisplay(dpy);
    return EXIT_SUCCESS;
}
