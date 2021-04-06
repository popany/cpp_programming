#pragma once

#define STATIC_CONCPP
#include <mysqlx/xdevapi.h>
#include <exception>
#include <iostream>

class MysqlAccessor
{
public:
    MysqlAccessor()
    {}

    void test()
    {
        try {
            mysqlx::Session sess("mysql_host", 33060, "root", "abc");
            mysqlx::Schema db = sess.getSchema("test");

            auto t = db.getTable("t_test_in");

            mysqlx::RowResult res = t.select("id", "name")
                .where("id > :param")
                .orderBy("id")
                .bind("param", 1).execute();

            std::cout << "total count: " << res.count() << std::endl;
            for (auto row = res.fetchOne(); !row.isNull(); row = res.fetchOne()) {
                std::cout << "id: " << row[0] << ", name: " << row[1] << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }
};

