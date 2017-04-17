#pragma once

#define SAFE_FREE(x) {if( x ) free( x ); (x)=NULL;}