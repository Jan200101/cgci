#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "context.h"
#include "build.h"
#include "ui.h"

#define CONTENT_TYPE_FORMAT "Content-Type: %s;\n\n"
#define TEXT_HTML "text/html"
#define TEXT_CSS "text/css"
#define TEXT_PLAIN "text/plain"

void print_html()
{
    if (current_project && current_project->name)
    {
        if (context.action)
        {
            if (!strcmp(context.action, "builds") && context.index)
            {
                if (context.extra)
                {
                    if (!strcmp(context.extra, "raw_log"))
                    {
                        printf(CONTENT_TYPE_FORMAT, TEXT_PLAIN);
                        printf("%s", current_build->log);
                    }
                    else if (!strcmp(context.extra, "log"))
                    {
                        printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
                        print_head();
                        printf(
                            "<pre>%s</pre>",
                            current_build->log
                        );
                    }
                    else
                    {
                        printf(CONTENT_TYPE_FORMAT, TEXT_PLAIN);
                        printf("Invalid path");
                    }

                }
                else
                {
                    printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
                    printf(HTML_START);
                    print_head();
                    print_title();
                    print_build_nav();
                    print_build_info();
                    print_footer();
                    printf(HTML_END);
                }
            }
            else if (!strcmp(context.action, "trigger"))
            {
                char* token = current_project->token;
                if (!token)
                    token = config.token;

                if (context.token && token && !strcmp(context.token, token))
                {
                    create_build();

                    printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
                    printf(HTML_START);
                    print_head();
                    print_footer();
                    printf(HTML_END);
                }
                else
                {
                    printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
                    printf(HTML_START);
                    print_head();
                    print_title();
                    print_build_nav();
                    print_build_trigger();
                    print_footer();
                    printf(HTML_END);
                }
            }
        }
        else
        {
            printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
            printf(HTML_START);
            print_head();
            print_title();
            print_build_nav();
            print_build_list();
            print_footer();
            printf(HTML_END);
        }
    }
    else
    {
        printf(CONTENT_TYPE_FORMAT, TEXT_HTML);
        printf(HTML_START);
        print_head();
        print_title();
        print_project_nav();
        print_project_list();
        print_footer();
        printf(HTML_END);
    }
}

void print_head()
{
    printf("<head>");

    printf("<meta charset=\"utf-8\">");

    printf("<title>CGCI");
    if (current_project && current_project->name)
    {
        printf(" : %s", current_project->name);
    }
    printf("</title>");

    char* token = config.token;
    if (current_project)
    {
        token = current_project->token;
        if (!token)
            token = config.token;
    }

    if (current_project && current_project->name
        && context.action && !strcmp(context.action, "trigger")
        && context.token && token && !strcmp(context.token, token))
        printf("<meta http-equiv=\"refresh\" content=\"0; url=/%s\"/>", current_project->name);

    printf("<link rel=\"stylesheet\" type=\"text/css\" href=\"%s/assets/base.css\"/>", config.path_prefix ? config.path_prefix : "");
    printf("</head>");
}

void print_title()
{
    printf("<h1 class=\"title\">");

    printf("<a ");
    if (context.project)
        printf("href=%s/", config.path_prefix ? config.path_prefix : "");
    printf(">CGCI</a>");

    if (context.project)
    {
        printf(" : <a href=\"%s/%s\">%s</a>", config.path_prefix ? config.path_prefix : "", context.project, context.project);
        if (context.action)
        {
            printf(" : %s", context.action);
        }
    }
    printf("</h1>");
}

void print_build_nav()
{
    printf(
        "<table class=\"tabs\">"
            "<tbody>"
                "<tr>"
                    "<td>"
                        "<a href=\"/%s\" class=\"active\">Builds</a>"
                    "</td>",
        current_project->name
    );

    if ((config.token || current_project->token) && current_project->script_path && strlen(current_project->script_path))
    {
        printf(
            "<td class=\"align-right\">"
                "<a href=\"/%s/trigger\">Trigger Build</a>"
            "</td>",
            current_project->name
        );
    }

    printf(
                "</tr>"
            "</tbody>"
        "</table>"
    );
}

void print_build_info()
{
    // YYYY-MM-DD HH:MM
    char time[18];
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M", gmtime(&current_build->timestamp));
    char buildtime[18];
    strftime(buildtime, sizeof(buildtime), "%Y-%m-%d %H:%M", gmtime(&current_build->completion));

    char* log_lines = current_build->log + strlen(current_build->log);
    int line_count = 20;

    while (line_count && log_lines > current_build->log)
    {
        --log_lines;

        if (*log_lines == '\n')
            --line_count;
    }

    printf(
        "<table class=\"build-info\">"
            "<tbody>"
                "<tr>"
                    "<td>Build ID</td>"
                    "<td>%s</td>"
                "</tr>"
                "<tr>"
                    "<td>Build Date</td>"
                    "<td>%s</td>"
                "</tr>"
                "<tr>"
                    "<td>Completion Time</td>"
                    "<td>%s</td>"
                "</tr>"
                "<tr>"
                    "<td>Status</td>"
                    "<td class=\"%s\">%s</td>"
                "</tr>"
                "<tr>"
                    "<td>Log</td>"
                    "<td>"
                        "<a href=\"/%s/builds/%s/raw_log\">Raw</a>"
                    "</td>"
                "</tr>"
                "<!--<tr>"
                    "<td>Artifact</td>"
                    "<td>"
                        "<a href=\"/polecat/builds/1/artifact\">Download</a>"
                    "</td>"
                "</tr>-->"
            "</tbody>"
        "</table>"
        "<pre class=\"log\">%s</pre>",
        current_build->name, time, current_build->completion ? buildtime : "",
        build_class[current_build->status], build_string[current_build->status],
        current_project->name, current_build->name,
        log_lines
    );
}

void print_build_trigger()
{
    printf(
        "<form>"
            "<label>Trigger Build</label><br>"
            "<input name=\"token\" type=\"password\" placeholder=\"token\" value=\"%s\" required><br>"
            "%s"
            "<input type=\"submit\" value=\"submit\">"
        "</form>",
        context.token ? context.token : "",
        context.token ? "Invalid token<br>" : ""
    );
}

void print_build_list()
{
    printf(
        "<table class=\"content\">"
            "<thead>"
                "<tr>"
                    "<th>Build ID</th>"
                    "<th>Build Date</th>"
                    "<th>Completion Time</th>"
                    "<th>Status</th>"
                "</tr>"
            "</thead>"
            "<tbody>"
    );

    for (size_t i = 0; i < current_project->build_count; ++i)
    {
        struct build_t* build = &current_project->builds[i];

        // YYYY-MM-DD HH:MM
        char time[18];
        strftime(time, sizeof(time), "%Y-%m-%d %H:%M", gmtime(&build->timestamp));
        char buildtime[255] = "";
        strdifftime(build->completion, build->timestamp, buildtime, sizeof(buildtime));

        printf(
            "<tr>"
                "<td>"
                    "<a  href=\"/%s/builds/%s\">%s</a>"
                "</td>"
                "<td>%s</td>"
                "<td>%s</td>"
                "<td class=\"%s\">%s</td>"
            "</tr>",
            context.project, build->name, build->name,
            time, buildtime,
            build_class[build->status], build_string[build->status]
        );
    }

    printf(
            "</tbody>"
        "</table>"
    );
}

void print_project_nav()
{
    printf(
        "<table class=\"tabs\">"
            "<tbody>"
                "<tr>"
                    "<td>"
                        "<a href=\"%s/\" class=\"active\">Projects</a>"
                    "</td>"
                "</tr>"
            "</tbody>"
        "</table>",
        config.path_prefix ? config.path_prefix : ""
    );
}

void print_project_list()
{
    printf(
        "<table class=\"content\">"
            "<thead>"
                "<tr>"
                    "<th>Name</th>"
                    "<th>Description</th>"
                    "<th>Last Build Date</th>"
                    "<th>Last Completion Time</th>"
                    "<th>Last Build Status</th>"
                "</tr>"
            "</thead>"
            "<tbody>"
    );

    for (size_t i = 0; i < config.project_count; ++i)
    {
        struct project_t* project = &config.projects[i];

        const char* class = "unknown";
        const char* status = "Unknown";
        char time[18] = "never";
        char buildtime[255] = "";
        if (project->build_count > 0)
        {
            class = build_class[project->builds[0].status];
            status = build_string[project->builds[0].status];
            strftime(time, sizeof(time), "%Y-%m-%d %H:%M", gmtime(&project->builds[0].timestamp));
            strdifftime(project->builds[0].completion, project->builds[0].timestamp, buildtime, sizeof(buildtime));
        }

        printf(
            "<tr>"
                "<td>"
                    "<a href=\"%s/%s\">%s</a>"
                "</td>"
                "<td>%s</td>"
                "<td>%s</td>"
                "<td>%s</td>"
                "<td class=\"%s\">%s</td>"
            "</tr>",
            config.path_prefix ? config.path_prefix : "",
            project->name, project->name,
            project->description ? project->description : "",
            time, buildtime,
            class, status
        );
    }
    printf(
            "</tbody>"
        "</table>"
    );
}

void print_footer()
{
    const time_t cur_time = time(NULL);

    char timestr[30] = "never";
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S %z", gmtime(&cur_time));

    printf(
        "<div class=\"footer\">"
            "<table class=\"tabs\">"
                "<tbody>"
                    "<tr>"
                        "<td>"
                        "</td>"
                    "</tr>"
                "</tbody>"
            "</table>"
            "<p class=\"text\">"
                "generated by cgci "VERSION" at %s"
            "</p>"
        "</div>",
        timestr
    );
}

void print_asset(const char* file)
{
    if (!file)
        return;

    printf(CONTENT_TYPE_FORMAT, TEXT_CSS);

    FILE* fd = fopen(file, "r");

    if (!fd)
        return;

    char buf[255+1];

    while (!feof(fd))
    {
        fread(buf, sizeof(buf)-1, sizeof(buf[0]), fd);
        buf[sizeof(buf)-1] = '\0';

        printf("%s", buf);
    }
}

void strdifftime(time_t time1, time_t time0, char* str, size_t size)
{
    if (!size)
        return;

    double diff = difftime(time1, time0);

    if (diff > 0)
    {
        *str = '\0';

        int seconds = (int)diff % 60;
        int minutes = (int)(diff / 60);
        int hours = minutes / 60;

        if (hours)
            snprintf(str+strlen(str), size, "%i hours ", hours);

        if (minutes)
            snprintf(str+strlen(str), size, "%i minutes ", minutes);

        if (hours || minutes || seconds)
            snprintf(str+strlen(str), size, "%i seconds ", seconds);
    }
}