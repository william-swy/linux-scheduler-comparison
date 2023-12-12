#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include "libpq-fe.h"

using namespace std::chrono_literals;

namespace {
    char query[] = "SELECT customer_id, order_count, average_amount FROM (SELECT customer_id, COUNT(order_id) AS order_count, AVG(total_amount) AS average_amount FROM (SELECT customer_id, order_id,total_amount FROM orders WHERE order_date BETWEEN '2022-03-01' AND '2022-10-01' UNION ALL SELECT NULL AS customer_id, generate_series(1, 1000000) AS order_id, 0 AS total_amount) AS subquery GROUP BY customer_id HAVING COUNT(order_id) >= 100) AS final_subquery ORDER BY average_amount DESC";

    std::chrono::duration<long double, std::milli> query_once(std::string conninfo) {
        PGconn *conn = PQconnectdb(conninfo.c_str());

        if (PQstatus(conn) != CONNECTION_OK) {
            std::cerr << "Error while connecting " << PQerrorMessage(conn) << '\n';

            PQfinish(conn);

            std::exit(1);
        }

        std::cout << "Port " << PQport(conn) << '\n';
        std::cout << "Host " << PQhost(conn) << '\n';
        std::cout << "DBName " << PQdb(conn) << '\n';

        std::this_thread::sleep_for(5s);

        const auto start{std::chrono::steady_clock::now()};
        PGresult *res = PQexec(conn, query);
        const auto end{std::chrono::steady_clock::now()};

        ExecStatusType res_status = PQresultStatus(res);

        if (res_status != PGRES_TUPLES_OK) {
            std::cerr << "Error query " << PQerrorMessage(conn) << '\n';

            PQclear(res);

            PQfinish(conn);

            std::exit(1);
        }

        PQclear(res);

        PQfinish(conn);

        return end - start;
    }

}


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Need host name\n";
        return 1;
    }
    std::string hostname{argv[1]};
    std::string conninfo = "dbname=test user=bigblue password=bigblue host="+ hostname +" port=5432";

    std::fstream log{"measurements"};

    log << std::fixed << std::setprecision(12);

    for (int i = 0; i < 100; i++) {
        const auto res = query_once(conninfo);
        log << res.count() << " ms\n";
        std::this_thread::sleep_for(5s);
        std::cout << "Iter: " << i << '\n';
    }
}