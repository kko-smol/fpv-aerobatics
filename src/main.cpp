
#include "app.h"


int main(){

    App app;

    if (app.init()){
        app.run();
    }

    return 0;
}
