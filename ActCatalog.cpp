#include "ActCatalog.h"
using namespace std;

map<string, ActDefinition> ActCatalog::build() {
    return {
        {"JOKE", {"JOKE", "Vous racontez une blague sur un clavier qui mange des nuages.", 25}},
        {"COMPLIMENT", {"COMPLIMENT", "Vous complimentez ses pixels: ils brillent de confiance.", 20}},
        {"INSULT", {"INSULT", "Vous critiquez sa coiffure binaire. Mauvaise idee.", -20}},
        {"DISCUSS", {"DISCUSS", "Vous lancez un debat sur les croissants quantiques.", 15}},
        {"OBSERVE", {"OBSERVE", "Vous observez calmement. Le monstre respire mieux.", 10}},
        {"PET", {"PET", "Vous tentez une caresse diplomatique. Surprise: ca marche.", 30}},
        {"OFFER_SNACK", {"OFFER_SNACK", "Vous offrez un snack. Silence croustillant.", 18}},
        {"REASON", {"REASON", "Vous argumentez avec logique et empathie.", 22}},
        {"DANCE", {"DANCE", "Vous dansez tres mal, mais avec conviction.", 12}},
        {"THREATEN", {"THREATEN", "Vous menacez le monstre. Il se fache instantanement.", -25}},
    };
}
