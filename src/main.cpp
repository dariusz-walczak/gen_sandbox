#include <iostream>
#include <cstdio>

#include <redland.h>


namespace {
    struct RedlandContext {
        librdf_world*   world;
        librdf_storage* storage;
        librdf_model*   model;
        librdf_parser*  parser;
        librdf_uri*     base_uri;
    };

    RedlandContext ReleaseRedland(const RedlandContext& ctx) {
        librdf_free_uri(ctx.base_uri);
        librdf_free_parser(ctx.parser);
        librdf_free_model(ctx.model);
        librdf_free_storage(ctx.storage);
        librdf_free_world(ctx.world);

        std::cout << "Released the Redland context" << std::endl;

        return {};
    }
}

int main() {
    RedlandContext redland_ctx = {};

    redland_ctx.world = librdf_new_world();

    if (!redland_ctx.world) {
        std::cerr
            << "Redland RDF Library: Failed to create the execution environment"
            << std::endl;

        return 1;
    }

    std::cerr << "Redland Context: Created a Redland World" << std::endl;

    librdf_world_open(redland_ctx.world);

    std::cerr << "Redland Context: Initialized the Redland World" << std::endl;

    // https://librdf.org/docs/api/redland-storage.html#librdf-new-storage
    redland_ctx.storage = librdf_new_storage(redland_ctx.world, "memory", nullptr, nullptr);

    if (!redland_ctx.storage) {
        std::cerr
            << "Redland Context: Failed to create a Redland Storage"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 2;
    }

    std::cerr << "Redland Context: Created a Redland Storage" << std::endl;

    // https://librdf.org/docs/api/redland-model.html#librdf-new-model
    redland_ctx.model = librdf_new_model(redland_ctx.world, redland_ctx.storage, nullptr);

    if (!redland_ctx.model) {
        std::cerr
            << "Redland Context: Failed to create a Redland Model"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 3;
    }

    std::cerr << "Redland Context: Created a Redland Model" << std::endl;

    // https://librdf.org/docs/api/redland-parser.html#librdf-new-parser
    redland_ctx.parser = librdf_new_parser(redland_ctx.world, "turtle", nullptr, nullptr);

    if (!redland_ctx.parser) {
        std::cerr
            << "Redland Context: Failed to create a Redland Parser"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 4;
    }

    std::cerr << "Redland Context: Created a Redland Parser" << std::endl;

    const unsigned char base_uri_str[] = "https://aurochsoft.com/";
    redland_ctx.base_uri = librdf_new_uri(redland_ctx.world, base_uri_str);

    if (!redland_ctx.base_uri) {
        std::cerr
            << "Redland Context: Failed to create the Base URI"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 5;
    }

    std::cerr << "Redland Context: Created the Base URI" << std::endl;

    const char* input_file_path = "sample.ttl";
    FILE* input_file = fopen(input_file_path, "r");

    if (!input_file) {
        std::cerr
            << "Failed to open the '" << input_file_path << "' file"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 6;
    }

    const int parser_error = librdf_parser_parse_file_handle_into_model(
        redland_ctx.parser, input_file, 0, redland_ctx.base_uri, redland_ctx.model);

    fclose(input_file);

    std::cerr << "Closed the '" << input_file_path << "' file" << std::endl;

    if (parser_error) {
        std::cerr
            << "Redland Context: Failed to parse the input file"
            << std::endl;

        redland_ctx = ReleaseRedland(redland_ctx);
        return 7;
    }

    std::cerr << "Redland Context: Successfully parsed the input file" << std::endl;

    librdf_model_print(redland_ctx.model, stdout);

    redland_ctx = ReleaseRedland(redland_ctx);

    return 0;
}
