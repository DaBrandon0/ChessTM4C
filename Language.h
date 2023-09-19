// Language.h

#ifndef LANGUAGE_H
#define LANGUAGE_H


enum Language {
	English = 0,
	Spanish = 1
};


enum StringTitle {
	String_LanguageName = 0,
	String_SelectLanguage = 1,
	String_SelectColor = 2,
	String_Black = 3,
	String_White = 4,
	String_YourMove = 5,
	String_WaitingForOpponent = 6,
	String_Check = 7,
	String_Checkmate = 8,
	String_Stalemate = 9,
	String_TimesUp = 10,
	String_YouWin = 11,
	String_YouLose = 12,
	String_PlayAgain = 13
};

char* GetTranslatedString(StringTitle stringTitle, Language language);



#endif