#include <iostream>
#include <string>
#include "libpq-fe.h"
#include <random>


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Need host name\n";
        return 1;
    }
    std::string hostname{argv[1]};
    std::string conninfo = "dbname=test user=bigblue password=bigblue host="+ hostname +" port=5432";

    // Create a connection
    PGconn *conn = PQconnectdb(conninfo.c_str());

    // Check if the connection is successful
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Error while connecting " << PQerrorMessage(conn) << '\n';

        PQfinish(conn);

        return 1;
    }

    std::cout << "Port " << PQport(conn) << '\n';
    std::cout << "Host " << PQhost(conn) << '\n';
    std::cout << "DBName " << PQdb(conn) << '\n';

    char drop_table_query[] = "DROP TABLE IF EXISTS orders";

    PGresult *res = PQexec(conn, drop_table_query);

    ExecStatusType res_status = PQresultStatus(res);

    if (res_status != PGRES_COMMAND_OK) {
        std::cerr << "Error dropping table " << PQerrorMessage(conn) << '\n';

        PQclear(res);

        PQfinish(conn);

        return 1;
    }

    std::cout << "Drop success\n";

    PQclear(res);

    char create_table_query[] = "CREATE TABLE IF NOT EXISTS orders (order_id INT PRIMARY KEY, customer_id INT, order_date DATE NOT NULL, total_amount INT)";

    res = PQexec(conn, create_table_query);
    res_status = PQresultStatus(res);

    if (res_status != PGRES_COMMAND_OK) {
        std::cerr << "Error creating table " << PQerrorMessage(conn) << '\n';

        PQclear(res);

        PQfinish(conn);

        return 1;
    }

    std::cout << "Create table success\n";

    PQclear(res);

    constexpr int entries{10000000};

    std::mt19937 gen{1234};

    std::uniform_int_distribution<> cust_id_dist{1, 10000};
    std::uniform_int_distribution<> amount_dist{0, 1000};

    std::uniform_int_distribution<> month_dist{1,11};
    std::uniform_int_distribution<> day_dist{1,20};


    for (int i = 1; i <= entries; i++) {
        const int order_id{i};
        const int customer_id{cust_id_dist(gen)};
        const int order_amount{amount_dist(gen)};
        
        const int month{month_dist(gen)};
        const int day{day_dist(gen)};

        std::string insert = "INSERT INTO orders(order_id, customer_id, order_date, total_amount) VALUES (" + std::to_string(order_id) + "," +  std::to_string(customer_id) + ", '2022-" + std::to_string(month) + "-" + std::to_string(day) + "'," + std::to_string(order_amount) + ")";

        res = PQexec(conn, insert.c_str());
        res_status = PQresultStatus(res);

        if (res_status != PGRES_COMMAND_OK) {
            std::cerr << "Error inserting value " << PQerrorMessage(conn) << '\n';

            PQclear(res);

            PQfinish(conn);

            return 1;
        }
        std::cout << "entry " << i << "inserted\n" ;

    }

    PQfinish(conn);
}