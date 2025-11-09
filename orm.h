#ifdef __CODE_GENERATOR__
#define TABLE(name) __attribute__((annotate("table:" #name)))
#define VIEW(name) __attribute__((annotate("view:" #name)))
#define IGNORE __attribute__((annotate("ignore")))
#define PK __attribute__((annotate("pk")))
#define FK __attribute__((annotate("fk")))
#define AUTOINC __attribute__((annotate("autoinc")))
#define READONLY __attribute__((annotate("readonly")))
#else
#define TABLE(name)
#define VIEW(name)
#define IGNORE
#define PK
#define FK
#define AUTOINC
#define READONLY
#endif