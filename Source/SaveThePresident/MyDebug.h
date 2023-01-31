#pragma once

#define Print(text) if(GEngine){ GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, text); }
#define PrintC(text, color) if(GEngine){ GEngine->AddOnScreenDebugMessage(-1, 0, color, text); }