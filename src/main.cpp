#include <iostream>

#include <redland.h>


int main() {
    librdf_world *world = librdf_new_world();

    if (!world) {
        std::cerr << "Failed to initialize a Redland RDF world" << std::endl;
        return 1;
    }

    librdf_world_open(world);
    std::cout << "Successfully initialized a Redland RDF world" << std::endl;

    librdf_free_world(world);
    std::cout << "Released the Redland RDF world." << std::endl;

    std::cout << "Hello, world!" << std::endl;
    return 0;
}
