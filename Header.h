//
//  Header.h
//  A5
//
//  Created by Jacky on 2017-11-19.
//  Copyright © 2017 Jacky. All rights reserved.
//

#ifndef Header_h
#define Header_h

typedef struct{
    sem_t sem;
    char * file_name_prefix;
    char * out_put_file;
}thread_arguments;

#endif /* Header_h */
