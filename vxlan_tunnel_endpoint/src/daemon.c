/*
 * Copryright (C) 2012-2013 NEC Corporation
 * NEC Confidential
 */

#include <assert.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "daemon.h"


#define PID_PATH "/var/run"


void
daemonize() {
  pid_t pid = fork();
  if ( pid < 0 ) {
    exit( 1 );
  }
  if ( pid > 0 ) {
    exit( 0 );
  }

  setsid();

  for ( int fd = getdtablesize(); fd >= 0; --fd ) {
    close( fd );
  }
  
  int fd = open( "/dev/null", O_RDWR ); // open stdin
  dup( fd ); // stdout
  dup( fd ); // stderr

  umask( S_IWGRP | S_IWOTH );

  chdir( "/tmp" );
}


static char *
get_pid_file_path( const char *name, char *path, size_t length ) {
  assert( name != NULL );
  assert( path != NULL );

  memset( path, '\0', length );
  snprintf( path, length - 1, "%s/%s.pid", PID_PATH, name );

  return path;
}


bool
pid_file_exists( const char *name ) {
  assert( name != NULL );
  assert( strlen( name ) > 0 );

  char path[ PATH_MAX ];
  struct stat st;
  int ret = stat( get_pid_file_path( name, path, sizeof( path ) ), &st );
  if ( ret < 0 ) {
    return false;
  }

  return true;
}


bool
create_pid_file( const char *name ) {
  assert( name != NULL );
  assert( strlen( name ) > 0 );

  if ( pid_file_exists( name ) ) {
    return false;
  }

  char path[ PATH_MAX ];
  get_pid_file_path( name, path, sizeof( path ) );

  FILE *fp = fopen( path, "w" );
  if ( fp == NULL ) {
    return false;
  }

  fprintf( fp, "%u", getpid() );

  fclose( fp );

  return true;
}


bool
remove_pid_file( const char *name ) {
  assert( name != NULL );
  assert( strlen( name ) > 0 );

  char path[ PATH_MAX ];
  int ret = unlink( get_pid_file_path( name, path, sizeof( path ) ) );
  if ( ret < 0 ) {
    return false;
  }

  return true;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
