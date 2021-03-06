//---------------------------------------------------------------------------
//--???????????? ?????, ???????, ?????????? ? ??. ????????? ??????????--//
//---------------------------------------------------------------------------
#define SHR_NAMES_COUNT     54
#define SHR1_NAMES_COUNT    24
#define SHR2_NAMES_COUNT    70
#define SHR3_NAMES_COUNT    25
#define SHR4_NAMES_COUNT    16
#define SHR5_NAMES_COUNT    10
#define SHR9_NAMES_COUNT    6
#define NORM_NAMES_COUNT    80
#define DIAGN_NAMES_COUNT   22 * 8
#define DIAGN_S_NAMES_COUNT 2 * 8
#define ZIN_NAMES_COUNT     3 * 16
#define OUT_NAMES_COUNT     3 * 16
#define AIK_NAMES_COUNT     2 * 8
#define AOUT_NAMES_COUNT    3 * 4

AnsiString SHRNames [ SHR_NAMES_COUNT + 1 ] =
{
    "",                                         // SHR0
    "??????? ??????",                           // SHR1
    "??????? ????",                             // SHR2
    "??????????",			                    // SHR3
    "????? ??",                                 // SHR4
    "?????????? ?????????",                     // SHR5
    "",                                         // SHR6
    "????? ?1",				                    // SHR7
    "????? ?2",				                    // SHR8
    "????????? ?????????? ?????????",           // SHR9
    "????? ?3",			                        // SHR10
    "",					                        // SHR11
    "",				                            // SHR12
    "",                                         // SHR13
    "",                                         // SHR14
    "",                                         // SHR15
    "",                                         // SHR16
    "",                                         // SHR17
    "",                                         // SHR18
    "",                                         // SHR19
    "???1",                                     // SHR20
    "???2",                                     // SHR21
    "",                                         // SHR22
    "???",				                        // SHR23
    "",					                        // SHR24
    "",					                        // SHR25
    "",					                        // SHR26
    "???. ??????",			                    // SHR27
    "????. ??????",			                    // SHR28
    "???. ???1",			                    // SHR29
    "???. ???2",			                    // SHR30
    "????. ???",			                    // SHR31
    "???. ??",				                    // SHR32
    "????. ??",				                    // SHR33
    "???. ???",				                    // SHR34
    "",					                        // SHR35
    "????. ?????? ??????",		                // SHR36
    "??????? ?????? ??",		                // SHR37
    "??????? ?????? ?? ? HOME",		            // SHR38
    "?? ? ???????????????",		                // SHR39
    "??????? ? ???.",		                    // SHR40
    "???????? ????????",		                // SHR41
    "???????? ? ???.",			                // SHR42
    "???????? ????????",		                // SHR43
    "?????? ?????",			                    // SHR44
    "?????? ????",			                    // SHR45
    "",					                        // SHR46
    "",					                        // SHR47
    "?????. ???? ????????",                     // SHR48
    "?????. ???? ????????",		                // SHR49
    "??????? ????????",		                    // SHR50
    "",					                        // SHR51
    "??????? ????????",			                // SHR52
    "?????? ?? ??????/?????"			        // SHR53
};

AnsiString SHR1Names [ SHR1_NAMES_COUNT + 1 ] =
{
    "",							                // 0
    "",	// 1
    "??????? ??-???",					        // 2
    "??????? ??",					            // 3
    "???????? ???????????? ?????",				// 4
    "??????? ??-??",					        // 5
    "???????????? ??????? ??",				    // 6
    "???. ????. ?????? ???",					// 7
    "??????? ??-??",		                    // 8
    "???????? ?????? ???.??????? ? ??????",		// 9
    "???????? ???????????? ?????",				// 10
    "??????? ??-???",		                	// 11
    "???????????? ??????? ??????",				// 12
    "??????? ??-???",					        // 13
    "????????? ???????????? ?????",				// 14
    "?????? ??",					            // 15
    "???????? ?????? ???.??????? ? ??????",		// 16
    "??????? ??",					            // 17
    "???????? ? ???????? ???????",			    // 18
    "??????????????? ??????? ??????",			// 19
    "???????? ???",				            	// 20
    "???????? ????",					        // 21
    "???????? ???",					            // 22
    "???????? ???????? ??????",					// 23
    "???????? ?????? ???.??????? ? ??"			// 24
};

AnsiString SHR2Names [ SHR2_NAMES_COUNT + 1 ] =
{
    "",                                                 // 0
    "??????? ??-???",                                   // 1
    "??????? ??, ??????? ??1, ??2, ??3",                // 2
    "???????? ?????? ?? ????? ???",                            // 3
    "?????? ? ?????? ?? ?????????",                     // 4
    "???????? ?????? ?????????",                 	// 5
    "???????? ???????? ??????",                    	// 6
    "???? ?????? <???????>",          		        // 7
    "???????? ?????? ???.??????? ? ??????",             // 8
    "???????? ?????? ?? ????? ???",                        // 9
    "???????? ????????? ????????:",                     // 10
    "?????????? ???",                                   // 11
    "???????? ? ??????? ??",                            // 12
    "???????? ?????? ?? ????? ??",                 	// 13
    "???????? ?????? ?? ????? ???1",          		// 14
    "???????? ?????? ?? ????? ??",      		// 15
    "???????? ????????? ????????: ",   	// 16
    "???????? ?????? ??????? ?? ? ???1",  		// 17
    "??????? ??",        				// 18
    "???????? ?????? ????? ???????",              	// 19
    "",  						// 20
    "?????????? ???",                        // 21
    "???????? ?????? ?? ????? ???",                     // 22
    "???????? ????????? ????????: ",           // 23
    "???????? ?????? ?????? ???",    			// 24
    "???????? ?????? ?? ????? ??", 			// 25
    "???????? ?????? ?? ????? ???2",                    // 26
    "???????? ?????? ?? ????? ???",                     // 27
    "???????? ? ??????? ??",                            // 28
    "???????? ????????? ?????? <????? ?1>",             // 29
    "???????? ????????? ???????? <????? ?1>: ",         // 30
    "???????? ? ??????? ?1",                  	// 31
    "???????? ????????? ???????? (?? ????.)",// 32
    "???????? ????????? ???????? (?? ???????): ",// 33
    "???????? ? ??????? ???",                            // 34
    "???????? ? ??????? ???",                 	// 35
    "???????? ????????? ?????? <????? ?2>",       	// 36
    "???????? ????????? ???????? <????? ?2>: ",         // 37
    "???????? ? ??????? ?2",                  		// 38
    "???????? ????????? ???????? (?? ????.)",// 39
    "???????? ????????? ???????? (?? ???????): ",// 40
    "???????? ? ??????? ??",                  	// 41
    "???????? ? ??????? ?1",                            // 42
    "???????? ?????? ?? ????? ???",             // 43
    "???????????? ???",         // 44
    "???????? ?????? ?? ????? ???",            	// 45
    "???????? ????????? ???????? <????? ???>: ",                           // 46
    "???????? ????????? ???????? (?? ????.)",// 47
    "???????? ????????? ???????? (?? ???????): ",                // 48
    "???????? ? ??????? ?2 ",                   //49
    "???????? ?????? ??????? ???, ???, ???2",   	// 50
    "??????? ??",   					// 51
    "???????? ?????? ????? ????????",   			// 52
    "???????? ?????? ?? ????? ???",   		// 53
    "???????? ????????? ????????: ",    			// 54
    "???????? ?????? ?????? ???",         //55
    "???????? ????????? ???????? (?? ????.)",	// 56
    "???????? ????????? ???????? (?? ???????): ",// 57
    "?????????? ???",					// 58
    "?????????? ???",					// 59
    "?????????? ????",					// 60
    "??????? ??, ??????? ??-???",			// 61
    "???????? ? ???????? ???????",			// 62
    "?????? ? ?????? ?? ?????????",                     // 63
    "???????? ??????? ??????",                          // 64
    "???????? ?????? ?? ????? ?1",			// 65
    "???????? ?????? ?? ????? ?2",                     // 66
    "???????? ?????? ?? ????? ?3",                          // 67
    "???????? ????????? ???????? (?? ???????): ",                                               //68
    "C???? ?????????? ?????? ???",                      //69
    "???????? ? ??????? ???"                                 // 70
};

AnsiString SHR3Names [ SHR3_NAMES_COUNT + 1 ] =
{
    "",                                             	// 0
    "",                     				// 1
    "???? ?????? <???????>",                        	// 2
    "???????? ?????? ?? ????? ??",                  	// 3
    "???????? ? ??????? ??",                        	// 4
    "???????? ?????? ?? ????? ???",                     // 5
    "???????? ?????? ?? ????? ???1",                    // 6
    "???????? ????????? ????????: ",    // 7
    "???????? ?????? ?????? ???",                 	// 8
    "???????? ?????? ?? ????? ??",                      // 9
    "???????? ? ??????? ???",                        // 10
    "???????? ?????? ?? ????? ???",                     // 11
    "???????? ?????? ?? ????? ???2",                    // 12
    "???????? ????????? ????????: ",    // 13
    "???????? ?????? ??????? ???",                // 14
    "?? ? ???????????????",         // 15
    "???????? ? ??????? ?1",                        // 16
    "???????? ?????? ?? ????? ???",                     // 17
    "???????? ?????? ?? ????? ???",                    // 18
    "???????????? ???",    // 19
    "???????? ?????? ?? ????? ???",                // 20
    "???????? ????????? ????????: ",            				// 21
    "??????? ??",             // 22
    "???????? ?????? ???.??????? ? ??????",                	// 23
    "?????????? ???"  ,                             	// 24
    "????? ?????????? ?????? ???"                         	// 25
};

AnsiString SHR4Names [ SHR4_NAMES_COUNT + 1 ] =
{
    "",                                         // 0
    "",          				// 1
    "???????? ?????? ?????? ???",               // 2
    "???????? ?????? ?????? ??",                // 3
    "???????? ?????? ??????? ???,???1,???2,???", 	// 4
    "?????????? ???",                           // 5
    "?????????? ???",                  		// 6
    "?????????? ????",                          // 7
    "",             // 8
    "???????? ?????? ?????????",                // 9
    "??????? ??-???",                  		// 10
    "??????? ??",                               // 11
    "???????? ? ???????? ???????",              // 12
    "?????? ? ?????? ?? ?????????",             // 13
    "???????? ??????? ??????",                  // 14
    "????? ?????????? ?????? ???",              // 15
    "?????????? ???"                            // 16
};

AnsiString SHR5Names [ SHR5_NAMES_COUNT + 1 ] =
{
    "",                                     // 0
    "??????? ??1, ??2, ??3, ??-???",        // 1
    "??????? ??",            		    // 2
    "??????? ??-???",                       // 3
    "???? ??",                       // 4
    "??????? ??-??",                       // 5
    "?????????? ????????????? ??????",                       // 6
    "?????????? ???",                       // 7
    "?????????? ???",                       // 8
    "?????????? ????",                      // 9
    "????? ?????????? ?????? ???"                      // 10
};

AnsiString SHR9Names [ SHR9_NAMES_COUNT + 1 ] =
{
    "",                                       // 0
    "?????? ??????????? ???????? ???????",    // 1
    "???????? ?????? ?????? ???",             // 2
    "???????? ?????? ?????? ????",            // 3
    "???????? ?????? ??????? ???,???1,???2,???",// 4
    "???? ?????? <????? ??>",                 // 5
    "???? ?????? <?????????? ?????????>"      // 6
};

AnsiString NormNames  [ NORM_NAMES_COUNT + 1 ]  =
{
    "                            ",         //(0)
    "??????? ??????? ??? ????????",         //(1)
    "??????? ??????? ??? ?????????",        //(2)
    "??? ????? ?? ?????",              	    //(3)
    "??? ????????",                   	    //(4)
    "??????? ?????? ?????????",             //(5)
    "?? ????????",                          //(6)
    "?????????? ????????? ?????????",       //(7)
    "???????? ?????????",                   //(8)
    "?????????? ?????????",                 //(9)
    "????? ???????? ????? ????????",        //(10)
    "?1 ?????????",                         //(11)
    "?2 ?????????",                         //(12)
    "?????????? ?????????",                 //(13)
    "????? ??? ????????",                   //(14)
    "???????? ???????? ???? ??????",        //(15)
    "???????? ????. ???????? ???? ??????",  //(16)
    "???????? ?/????????? ???? ??????",     //(17)
    "????? ???1 ????????",                  //(18)
    "????? ???2 ????????",                  //(19)
    "????? ?3 ????????",                    //(20)
    "????? ?? ????????",                    //(21)
    "????? ??? ????????",                   //(22)
    "????? ?1 ????????",                    //(23)
    "????? ?2 ????????",                    //(24)
    "???1 ????????",                        //(25)
    "???2 ????????",                        //(26)
    "????? ????? ????????",                 //(27)
    "???????? ???????? ? ????????",         //(28)
    "???????? ????.???????? ? ????????",    //(29)
    "???????? ??????? ????????",            //(30)
    "???????? ??????? ?????????",           //(31)
    "????????????????? ? ????????",         //(32)
    "????????? ?????????? ????????? ?????????", //(33)
    "??-??? ????????",                      //(34)
    "??-??? ????????",                      //(35)
    "??-?? ????????",                       //(36)
    "??-?? ????????",                       //(37)
    "?????? ????????",                      //(38)
    "?????? ????????",                      //(39)
    "?? ?????????",                         //(40)
    "?? ?????????",                         //(41)
    "???????? ?????????",                   //(42)
    "???????? ??????????",                  //(43)
    "????. ?/????. ????????",               //(44)
    "????. ?/????. ?????????",              //(45)
    "????????: 360 ???? = ",                //(46)
    "?/????????: 360 ???? = ",              //(47)
    "?/?????????: 360 ???? = ",             //(48)
    "??.???? ???????? ????????",            //(49)
    "??.???? ?/????. ????????",             //(50)
    "?? ???????",                           //(51)
    "?? ????????",                          //(52)
    "???1 ????? ?? ??????",                 //(53)
    "???2 ????? ?? ??????",                 //(54)
    "?3 ?????????",                         //(55)
    "??-?3 ????????",                       //(56)
    "??-?4 ????????",                       //(57)
    "??-?4 ????????",                       //(58)
    "??? ????? ????? ?? ?????",             //(59)
    "????? ??? ????????",             //(60)
    "??? ????? ?? ?????",          //(61)
    "????? ??? ???????? ????????",          //(62)
    "????. ?? ? ?????. ?????. ??????",      //(63)
    "????. ?? ? ?????. ?????. ??????",	    //(64)
    "?? ?????? ? ???????",	            //(65)
    "????? ?????????? ????????",             //(66)
    "?? ? ???????????????",                   //(67)
    "?????. ???????? ????????? ????????",                   //(68)
    "?????. ???????? ?????? ? ????????",                   //(69)
};

AnsiString DiagnNames [ DIAGN_NAMES_COUNT ] =
{
//-------------------------------------------[0]
 "??? ?????? ?? ????? ???",                     //(0x01)
 "??????? ?????? ????????????? ???",            //(0x02)
 "??? ????????????? ???",                       //(0x04)
 "??? ????????? ???",                           //(0x08)
 "??? ?????????? ???",                          //(0x10)
 "??? ????????? ???????? ???????? ???",         //(0x20)
 "??? ?????????? ???????? ???????? ???",        //(0x40)
 "",                                            //(0x80)
//-------------------------------------------[1]
 "??? ?????? ?? ????? ???(????????)",           //(0x01)
 "??????? ?????? ????????????? ???",            //(0x02)
 "??? ????????????? ???",                       //(0x04)
 "??????? ??????? ??? ?? ????????",             //(0x08)
 "??? ?????? ?? ????????? ??????? ???",         //(0x10)
 "??? ?????? ?? ?????????? ??????? ???",        //(0x20)
 "??? ?????? ?? ????? ??????????? ???",         //(0x40)
 "??????? ??????? ??? ?? ?????????",		    //(0x80)
//-------------------------------------------[2]
 "??? ?????? ?? ????? ??",                      //(0x01)
 "??????? ?????? ????????????? ??",             //(0x02)
 "??? ????????????? ??",                        //(0x04)
 "??? ????????? ????? ??????? ??",              //(0x08)
 "??? ?????????? ????? ??????? ??",             //(0x10)
 "??? ????????? ????????? ?????????? ??",       //(0x20)
 "??? ?????????? ????????? ?????????? ??",     //(0x40)
 "[2] 0x80",                                    //(0x80)
//-------------------------------------------[3]
 "??? ??????????? ?3",				            //(0x01)
 "[3] 0x02",					                //(0x02)
 "???????? ???????? ??? ?? ?????????",          //(0x04)
 "??? ??????????? ?1",                          //(0x08)
 "??? ??????????? ?2",                          //(0x10)
 "??? ?????????? ??????????? ???????????",      //(0x20)
 "????????? ?? ??????? ??????",                 //(0x40)
 "????????? ?????????? ??????",                 //(0x80)
//-------------------------------------------[4]
 "???????? ???????? ?? ? ???????? ?????????",   //(0x01)
 "??? ?????? ????????? ???????? ?? ????????? ?????????",	//(0x02)
 "????? ????? ???????? ????????? ????????",     //(0x04)
 "???????: ???????????? ????????? ??????? ????",    //(0x08)
 "???????: ?????????? ????????? ? ???????? ????",    //(0x10)
 "??? ???????? ????????",                     	//(0x20)
 "[4] 0x40",	                                //(0x40)
 "?????? ?? ???????",                           //(0x80)
//-------------------------------------------[5]
 "???????? ???????? ?? ? ???????? ?????????",   //(0x01)
 "??? ?????? ????????? ???????? ?? ????????? ?????????",  //(0x02)
 "????? ????? ???????? ????????? ????????",     //(0x04)
 "????????: ???????????? ????????? ???????? ????",//(0x08)
 "????????: ?????????? ????????? ? ???????? ????",//(0x10)
 "[5] 0x20",                                    //(0x20)
 "[5] 0x40",                                    //(0x40)
 "[5] 0x80",                                    //(0x80)
//-------------------------------------------[6]
 "???????? ????/???????? ?? ? ??????? ?????????",//(0x01)
 "??? ?????? ????????? ????/???????? ?? ????????? ?????????",//(0x02)
 "????? ????? ???????? ????????? ????/????????", //(0x04)
 "?/????????: ???????????? ????????? ???????? ????",  //(0x08)
 "?/????????: ?????????? ????????? ? ???????? ????",  //(0x10)
 "[6] 0x20",                                     //(0x20)
 "?????? ?? ????????? ?????",                    //(0x40)
 "?????? ?? ?????????? ????",                    //(0x80)
//-------------------------------------------[7]
 "??? ?????? ?? ????? ???1",                    //(0x01)
 "??????? ?????? ????????????? ???1",           //(0x02)
 "??? ????????????? ???1",                      //(0x04)
 "[7] 0x08",               			            //(0x08)
 "??? ?????? ?? ????? ???2",           		    //(0x10)
 "??????? ?????? ????????????? ???2",           //(0x20)
 "??? ????????????? ???2",                      //(0x40)
 "[7] 0x80",                                    //(0x80)
//-------------------------------------------[8]
 "??? ?????? ?? ????? ???",                       //(0x01)
 "??????? ?????? ????????????? ???",            //(0x02)
 "??? ????????????? ???",                       //(0x04)
 "[8] 0x08",              			//(0x08)
 "??? ?????? ?? ????? ???3",           		//(0x10)
 "??????? ?????? ????????????? ???3",           //(0x20)
 "??? ????????????? ???3",                      //(0x40)
 "[8] 0x80",                                    //(0x80)
//-------------------------------------------[9]
 "??? ???????? ? ?????????",                    //(0x01)
 "??? ?????????? ???????????",                  //(0x02)
 "??? ?????????? ??",            //(0x04)
 "??? ?????????? ??????? ? ??????? ?????? ??????",                       //(0x08)
 "??????? ??????????? ???? ?????????? ?1 ? ?2",	//(0x10)
 "??????? ??????????? ???? ?????????? ??",//(0x20)
 "??????? ??????????? ???? ???. ??????? ? ??????? ?????? ??????",  	//(0x40)
 "??? ???????? ? ??????????",                   //(0x80)
//-------------------------------------------[10]
 "????? ????????? ???????? ????????",           //(0x01)
 "????? ????????? ???????? ????????",        	//(0x02)
 "????? ????????? ???????? ????????",           //(0x04)
 "????? ????????? ???????? ????/????????",      //(0x08)
 "????????? ?????????? ??",  					//(0x10)
 "??????? ??????????? ???? ?????????? ?????? ?????? ??????",	//(0x20)
 "??? ?????????? ??????????? ??",		//(0x40)
 "?? ??????????",                    	//(0x80)
//-------------------------------------------[11]
 "??????: ??? ?????????? ???????????",          //(0x01)
 "??????: ??? ?????????? ??",	//(0x02)
 "??????: ??????? ???????? ?? ????? ???",		    //(0x04)
 "??????: ??? ?????????? ??????????? ??",       //(0x08)
 "??????: ?????????? ???",			            //(0x10)
 "??????: ??????",                              //(0x20)
 "??????: ??????? ???????? ? ??????",		    //(0x40)
 "??????: ??? ???????? ????????",		        //(0x80)
//-------------------------------------------[12]
 "??-?? ?? ????????",     			//(0x01)
 "??-?? ?? ????????",    			//(0x02)
 "????????? ??-?? ?? ??????????",            	//(0x04)
 "????????? ??-?? ????????????",           	//(0x08)
 "??: ??? ?????? ?? ?????????? ?? ????????",     	//(0x10)
 "??: ??? ?????? ?? ?????????? ?? ?????????",    	//(0x20)
 "??: ??? ?????? ?? ??????? '????'",		//(0x40)
 "?? ?? ?????? ? ???????? ?????????",           //(0x80)
//-------------------------------------------[13]
 "??-??? ?? ????????",                          //(0x01)
 "??-??? ?? ????????",                          //(0x02)
 "????????? ??-??? ?? ??????????",              //(0x04)
 "????????? ??-??? ????????????",               //(0x08)
 "??? ?????????? ?????? ?????? ??????",                         		//(0x10)
 "??????? ??????????? ???? ?????????? ??????????? ??",                          		//(0x20)
 "?? ?? ?????????",                             //(0x40)
 "?? ?? ?????????",                             //(0x80)
//-------------------------------------------[14]
 "????????? ?????? ??",                           	//(0x01)
 "?? ?? ????????",                           	//(0x02)
 "????????? ?? ?? ??????????",               	//(0x04)
 "????????? ?? ????????????",                	//(0x08)
 "[14] 0x10",                  			//(0x10)
 "[14] 0x20",                  			//(0x20)
 "??? ?? ?????????",                           	//(0x40)
 "??? ?? ??????????",                           //(0x80)
//-------------------------------------------[15]
 "??? ?????? ???",                           	//(0x01)
 "??? ????????? ???",                           //(0x02)
 "??? ?????? ?? ????? ???",             	//(0x04)
 "??? ?????????? ???",                		//(0x08)
 "??????? ????? ????????? ???",          	//(0x10)
 "[15] 0x20",  					//(0x20)
 "?? ?? ?????????",				//(0x40)
 "?? ?? ??????????",				//(0x80)
//-------------------------------------------[16]
	"??? ?????????? ??????? ??", 			// 0x01
	"??? ???????? ??????? ??",				// 0x02
	"??? ?????? ?? ??????? ??????? ??",		// 0x04
	"??? ?????? ???????? ??????? ??",		// 0x08
	"?????? ?? ?? ?????? ? HOME",	    	// 0x10
	"??? ?????????? ???????? ??????? ??",	// 0x20
	"?????? ??????? ??",
	"",
//-------------------------------------------[17]
 "[17] 0x01",					//(0x01)
 "[17] 0x02",					//(0x02)
 "??: ???? ???????",				//(0x04)
 "??: ??????? ??????????? ???????",			//(0x08)
 "??: ??????? ??????????? ????",			//(0x10)
 "??: ?????? ????? ????",			//(0x20)
 "??: ?????? ???????? ?????",			//(0x40)
 "??: ?????? ??????????? ?????",			//(0x80)
//-------------------------------------------[18]
 "??? ?????? ?? ???????? ?/????????",           //(0x01)
 "??? ?????? ?? ???????? ?/????????",           //(0x02)
 "??? ?????? ???????? ???????? ?? ?/????????",  //(0x04)
 "??? ?????? ???????? ??????? ?? ?/????????",   //(0x08)
 "??? ?????? ?? ??????? ????? ????????? ?/????????",//(0x10)
 "??? ?????? ?? ????? ?/????????",              //(0x20)
 "??????? ?????? ????????????? ?/????????",     //(0x40)
 "??? ????????????? ?/????????",                	//(0x80)
//-------------------------------------------[19]
 "??????: ?????? ??",                           //(0x01)
 "??????: ???",                                 //(0x02)
 "  ",  //(0x04)
 "  ",   //(0x08)
 "  ",//(0x10)
 "  ",              //(0x20)
 "  ",     //(0x40)
 "  ",                	//(0x80)
//-------------------------------------------[20]
 "???: ????????",           //(0x01)
 "???: ??????????????",           //(0x02)
 "??? ???????????? ???",  //(0x04)
 "  ",   //(0x08)
 "?????? ???????? Ar",//(0x10)
 "??????? ???????? Ar",              //(0x20)
 "?????? ???????? N2",     //(0x40)
 "??????? ???????? N2",                	//(0x80)
//-------------------------------------------[21]
"??? ?????? ?? ????? ???",
"??????? ?????? ????????????? ???",
"??? ????????????? ???",
"??? ???????????? ???",
"?????????? ?????? ??? ?? ???????????",
"?????????? ?????? ??? ?? ???????",
"??? ?????????? ???",
"?????????? ??????????? ???"
};

AnsiString DiagnSNames [ DIAGN_S_NAMES_COUNT ] =
{
    // 0 ????
    "??? ????? ? ???. ???????? ?1",             //(0x01)
    "??? ????? ? ???. ???????? ?2",             //(0x02)
    "??? ????? ? ?????????? (???)",             //(0x04)
    "??? ????? ? ???",                          //(0x08)
    "??? ????? ? ????",                           //(0x10)
    "??? ????? ? ??",                                         //(0x20)
    "S[0] 0x40",                                         //(0x40)
    "?????????????? ???????",                   //(0x80)
    // 1 ????
    "??? ????? ? ISO-P32C32",                   //(0x01)
    "??? ????? ? PEX-P16R16(1)",                      //(0x02)
    "??? ????? ? PEX-P16R16(2)",                      //(0x04)
    "??? ????? ? ISO-813",                     //(0x08)
    "??? ????? ? ISO-DA16",                                     //(0x10)
    "S[1] 0x20",                                     //(0x20)
    "S[1] 0x40",                 				//(0x40)
    "S[1] 0x80"                                      //(0x80)
};

AnsiString zinNames [ ZIN_NAMES_COUNT ] =
{
    "DI0 ???? ?????????? ???????????",                		// zin0_0
    "DI1 ???? ?????????? ??????? ??",						// zin0_1
    "DI2 ???? ???. ??????? ? ????. ?????? ??????",          // zin0_2
    "DI3 ???? ?????????? ?????? ?????? ??????",           	// zin0_3
    "DI4 ????? ????????? ???????? ????????",				// zin0_4
    "DI5 ????? ????????? ???????? ????????",				// zin0_5
    "DI6 ???? ???????? ? ??????????",						// zin0_6
    "DI7 ??????? ??????? ??????????? ????????",				// zin0_7
    "DI8   ????????? ????????? 1",							// zin0_8
    "DI9   ????????? ????????? 2",							// zin0_9
    "DI10 ???? ?????????? ??????????? ??",					// zin0_10
    "DI11 ",												// zin0_11
    "DI12 ??-??? ??????",									// zin0_12
    "DI13 ??-??? ??????",									// zin0_13
    "DI14 ??-?? ??????",									// zin0_14
    "DI15 ??-?? ??????",                        			// zin0_15

    "DI0 ??? ???????",										// zin1_0
    "DI1 ",													// zin1_1
    "DI2 ?????? ????????? ????????? ????????",				// zin1_2
    "DI3 ?????? ????????? ????????? ????????",				// zin1_3
    "DI4 ?????? ?????(??????)",								// zin1_4
    "DI5 ?????? ??????(??????)",							// zin1_5
    "DI6 ?????? ???. ??????(?????????????)",				// zin1_6
    "DI7 ",                                     			// zin1_7
    "DI8   ???????? ???????? ??? ?????????",				// zin1_8
    "DI9   ???????? ???????? ???? ?????????",				// zin1_9
    "DI10 ?????????? ??????? ??",							// zin1_10
    "DI11 ?????? ?? ? HOME",                      			// zin1_11
    "DI12 ??? ?????? ??????? ??",            				// zin1_12
    "DI13 ",                 								// zin1_13
    "DI14 ?? ??????",                          				// zin1_14
    "DI15 ?? ??????",                          				// zin1_15

    "DI0 ???: ???? ?????????? ??????",	                    // zin2_0
    "DI1 ???: ??????????",                     				// zin2_1
    "DI2 ???: ????????",   									// zin2_2
    "DI3 ???: ??????????????",         						// zin2_3
    "DI4 ???: ??????",										// zin2_4
    "DI5 ???: ???????? ??????.(??????????)",			    // zin2_5
    "DI6 ",													// zin2_6
    "DI7 ",													// zin2_7
    "DI8 ",               									// zin2_8
    "DI9 ",													// zin2_9
    "DI10 ",                     							// zin2_10
    "DI11 ",                          						// zin2_11
    "DI12 ",												// zin2_12
    "DI13 ",												// zin2_13
    "DI14 ",												// zin2_14
    "DI15 "                         						// zin2_15
};

AnsiString outNames [ OUT_NAMES_COUNT ] =
{
    "DO0 ???????? ???",            							// out0_0
    "DO1 ???????? ???????? ??????",             			// out0_1
    "DO2 ?????????? ?????????",          					// out0_2
    "DO3 ???????? ???????? ??????",            				// out0_3
    "DO4 ??????? ??-???",                       			// out0_4
    "DO5 ??????? ??-??",                        			// out0_5
    "DO6 ",         										// out0_6
    "DO7 ",         										// out0_7
    "DO8   ?????????? ????????? 1",      	       			// out0_8
    "DO9   ?????????? ????????? 2",       					// out0_9
    "DO10 ",              									// out0_10
    "DO11 ???? ??????? ???????? ??",               			// out0_11
    "DO12 ???????? ? HOME ??????? ??",             			// out0_12
    "DO13 ???? ??",                                 		// out0_13
    "DO14 ????? ?????? ??",                         		// out0_14
    "DO15 ",                                    			// out0_15

    "DO0 ???????? ????????? ??????? ??????",            	// out1_0
    "DO1 ???????? ?????? ????????? ??????",            		// out1_1
    "DO2 ???????? ??????? ??????? ???????????",            	// out1_2
    "DO3 ????. ??????? ??????? ???????????",             	// out1_3
    "DO4 ",      											// out1_4
    "DO5 ??????? ?????? ???1 ? ??????(??1)",                // out1_5
    "DO6 ??????? ?????? ???2 ? ??????(??2)",                // out1_6
    "DO7 ??????? ?????? ???1 ? ?? (??3)",               	// out1_7
    "DO8   ???????? ??????? ?????????? ???",                // out1_8
    "DO9   ????????? ???????? ???????? ???",                // out1_9
    "DO10 ???????? ??????? ?????????? ????",                // out1_10
    "DO11 ????????? ???????? ???????? ????",            	// out1_11
    "DO12 ",                   								// out1_12
    "DO13 ??????? ?????? ??????? ? ??????. ???.",           // out1_13
    "DO14 ??????? ?????? ?. ??????? ????? ? ???.",          // out1_14
    "DO15 ??????? ?????? ???????",              			// out1_15

    "DO0 ??: ????",		                					// out2_0
    "DO1 ",                       							// out2_1
    "DO2 ???: ???????? ?????????? ??????",                  // out2_2
    "DO3 ???: ???????? ????. ??????????",                   // out2_3
    "DO4 ???: ???????? ???????? ????????",                  // out2_4
    "DO5 ???: ????? ??????",                     		    // out2_5
    "DO6 ???: ????. ??????????? ??????????????",            // out2_6
    "DO7 ",               									// out2_7
    "DO8 ???????? ????????????: ???????",                   // out2_8
    "DO9 ???????? ????????????: ??????",                    // out2_9
    "DO10 ???????? ????????????: ???????",                  // out2_10
    "DO11 ???????? ????????????",                           // out2_11
    "DO12 ",                                   				// out2_12
    "DO13 ",                                   				// out2_13
    "DO14 ",                                   				// out2_14
    "DO15 "                                    				// out2_15
};

AnsiString aikNames [ AIK_NAMES_COUNT ] =
{
    "AIK0 ????. ???? ?????????? ???????????",   			// aik0
    "AIK1 ????. ???? ?????????? ??????? ??",  				// aik1
    "AIK2 ????. ???? ???. ??????? ? ????. ??. ???.",		// aik2
    "AIK3 ????. ???? ???. ?????? ?????? ??????", 			// aik3
    "AIK4 ?????? ???1(Ar)",                         		// aik4
    "AIK5 ?????? ???2(N2)",                         		// aik5
    "AIK6 ?????????????",                       			// aik6
    "AIK7 ???????? Ar ? ????? ???1",           				// aik7
    "AIK8 ???????? N2 ? ????? ???2",           				// aik8
    "AIK9 ????. ???? ?????????? ??????????? ??",           	// aik9
    "AIK10 ???????? ???? ?? ????? ?????????",           	// aik10
    "AIK11 ????????? ???????????? ????? ?? ???",           	// aik11
    "AIK12 ????????? ???????????? ????? ?? ???",            // aik12
    "AIK13 ???????? ????????",           					// aik13
    "AIK14 ?????????? ????????",           					// aik14
    "AIK15 ??????????? ??"           						// aik15
};


AnsiString aoutNames [ AOUT_NAMES_COUNT ] =
{
    "A_OUT0 ??????. ?????????? ??????? ??????", 			// aout0
    "A_OUT1 ??????. ????????? ????. ????????", 				// aout1
    "A_OUT2 ??????. ????????? ????. ????????", 				// aout2
    "A_OUT3 ",                                  			// aout3
    "A_OUT4 ??????? ???1",                      			// aout4
    "A_OUT5 ??????? ???2",                      			// aout5
    "A_OUT6 ??????? ???. ????. ????? ?? ???",               // aout6
    "A_OUT7 ??????? ???. ????. ????? ?? ???",  				// aout7
    "A_OUT8 ??????? ???????? ???",  						// aout8
    "A_OUT9 ",  											// aout9
    "A_OUT10 ",  											// aout10
    "A_OUT11 "  											// aout11
};

