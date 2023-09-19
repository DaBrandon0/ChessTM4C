// Language.cpp

#include "Language.h"

char stringData[14][2][50] = {
    {
        "English",
        "Espa\xA4ol"
    },
    {
        "Select language",
        "Selecciona un idioma"
    },
    {
        "Select color",
        "Seleccionar el color"
    },
        {
        "Black",
        "Negro"
    },
        {
        "White",
        "Blanco"
    },
        {
        "Your move",
        "Tu turno"
    },
        {
        "Waiting for opponent",
        "Esperando para oponente"
    },
        {
        "Check",
        "Jaque"
    },
    {
        "Checkmate",
        "Mate"
    },
    {
        "Stalemate",
        "Estancamiento"
    },
    {
        "Time's up",
        "Se acab\xA2 el tiempo"
    },
        {
        "You won",
        "Ganaste"
    },
        {
        "You lost",
        "Perdiste"
    },
    {
        "Press any button to play again",
        "Presiona cualquier bot\xA2n para volver a jugar"
    }
};

char* GetTranslatedString(StringTitle stringTitle, Language language) {
	return stringData[stringTitle][language];
}