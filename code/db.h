typedef enum
  {
   META_COMMAND_SUCCESS = 0,
   META_COMMAND_EXIT,
   META_COMMAND_UNRECOGNIZED_COMMAND
  } meta_command_result;

typedef enum
  {
   PREPARE_SUCCESS = 0,
   PREPARE_SYNTAX_ERROR,
   PREPARE_UNRECOGNIZED_STATEMENT
  } prepare_result;

typedef enum
  {
   STATEMENT_INSERT = 0,
   STATEMENT_SELECT
  } statement_type;

typedef enum
  {
   EXECUTE_SUCCESS = 0,
   EXECUTE_TABLE_FULL
  } execute_result;

typedef struct
{
  void* Memory;
  size_t BufferLen;
  ssize_t InputLen;
} input_buffer;

typedef struct
{
  void* TableMemory;
  size_t TableMemorySize;
  
  void* TransientMemory;
  size_t TransientMemorySize;
} program_memory;

typedef struct
{
  uint32 Id;
  char Username[COLUMN_USERNAME_SIZE];
  char Email[COLUMN_EMAIL_SIZE];
} row;

typedef struct
{
  uint32 NumOfRows;
  void* Pages[TABLE_MAX_PAGES];
} table;

typedef struct
{
  statement_type Type;
  row RowToInsert;
} statement;
