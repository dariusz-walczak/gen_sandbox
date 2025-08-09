#include <iostream>
#include <cstdio>

#include <redland.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "common/common_exception.hpp"
#include "common/redland_utils.hpp"


int main() {
    spdlog::set_level(spdlog::level::info);
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr_logger"));

    common::scoped_redland_ctx redland_ctx = common::create_redland_ctx();

    try
    {
        initialize_redland_ctx(redland_ctx);
    }
    catch (const common::common_exception& e)
    {
        spdlog::critical("{}: {}", __func__, e.what());

        return 1;
    }

    common::load_rdf(redland_ctx->world, redland_ctx->model, "data/people/batch1.ttl");
    common::load_rdf(redland_ctx->world, redland_ctx->model, "data/people/batch2.ttl");

    std::cout << '\n';

    librdf_model_print(redland_ctx->model, stdout);

    std::cout << '\n';

    const std::string query = {
        R"(
        PREFIX gx: <http://gedcomx.org/>

        SELECT ?person, ?name, ?gender, ?birthDate, ?deathDate
        WHERE {
            ?person a gx:Person ;
                gx:name ?name .
            OPTIONAL {
                ?person gx:gender ?gender
            }
            ?person gx:birthDate ?birthDate .
            OPTIONAL {
                ?person gx:deathDate ?deathDate
            }

        })"
    };

    common::exec_query(redland_ctx->world, redland_ctx->model, query);

    return 0;
}
