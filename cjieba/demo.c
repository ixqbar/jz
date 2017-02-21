#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/jieba.h"

void CutDemo()
{
  printf("CutDemo:\n");
  static const char *DICT_PATH = "./dict/jieba.dict.utf8";
  static const char *HMM_PATH = "./dict/hmm_model.utf8";
  static const char *USER_DICT = "./dict/user.dict.utf8";
  // init will take a few seconds to load dicts.
  Jieba handle = NewJieba(DICT_PATH, HMM_PATH, USER_DICT);

  const char *s = "杨振宁、姚期智放弃外国国籍 转为中科院院士";
  size_t len = strlen(s);
  CJiebaWord *words = Cut(handle, s, len);
  CJiebaWord *x;
  for (x = words; x && x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);
  FreeJieba(handle);
}

void CutForSearchDemo()
{
  printf("CutForSearchDemo:\n");
  static const char *DICT_PATH = "./dict/jieba.dict.utf8";
  static const char *HMM_PATH = "./dict/hmm_model.utf8";
  static const char *USER_DICT = "./dict/user.dict.utf8";
  // init will take a few seconds to load dicts.
  Jieba handle = NewJieba(DICT_PATH, HMM_PATH, USER_DICT);

  const char *s = "杨振宁、姚期智放弃外国国籍 转为中科院院士";
  size_t len = strlen(s);
  CJiebaWord *words = CutForSearch(handle, s, len);
  CJiebaWord *x;

  for (x = words; x && x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);
  FreeJieba(handle);
}

void CutWithoutTagNameDemo()
{
  printf("CutWithoutTagNameDemo:\n");
  static const char *DICT_PATH = "./dict/jieba.dict.utf8";
  static const char *HMM_PATH = "./dict/hmm_model.utf8";
  static const char *USER_DICT = "./dict/user.dict.utf8";
  // init will take a few seconds to load dicts.
  Jieba handle = NewJieba(DICT_PATH, HMM_PATH, USER_DICT);

  const char *s = "二货物们欢迎到来并献上鲜花，OMG！";
  size_t len = strlen(s);
  CJiebaWord *words = CutWithoutTagName(handle, s, len, "x");
  CJiebaWord *x;
  for (x = words; x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);
  FreeJieba(handle);
}

void ExtractDemo()
{
  printf("ExtractDemo:\n");
  static const char *DICT_PATH = "./dict/jieba.dict.utf8";
  static const char *HMM_PATH = "./dict/hmm_model.utf8";
  static const char *IDF_PATH = "./dict/idf.utf8";
  static const char *STOP_WORDS_PATH = "./dict/stop_words.utf8";
  static const char *USER_DICT = "./dict/user.dict.utf8";

  // init will take a few seconds to load dicts.
  Extractor handle = NewExtractor(DICT_PATH,
                                  HMM_PATH,
                                  IDF_PATH,
                                  STOP_WORDS_PATH,
                                  USER_DICT);

  const char *s = "二货物们欢迎到来并献上鲜花，OMG！";
  size_t top_n = 20;
  CJiebaWord *words = Extract(handle, s, strlen(s), top_n);
  CJiebaWord *x;
  for (x = words; x && x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);
  FreeExtractor(handle);
}

void UserWordDemo()
{
  printf("UserWordDemo:\n");
  static const char *DICT_PATH = "./dict/jieba.dict.utf8";
  static const char *HMM_PATH = "./dict/hmm_model.utf8";
  static const char *USER_DICT = "./dict/user.dict.utf8";
  Jieba handle = NewJieba(DICT_PATH, HMM_PATH, USER_DICT);

  const char *s = "二货物们欢迎到来并献上鲜花，OMG！";
  size_t len = strlen(s);

  CJiebaWord *words = Cut(handle, s, len);
  CJiebaWord *x;
  for (x = words; x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);

  JiebaInsertUserWord(handle, "OM");
  JiebaInsertUserWord(handle, "二货");
  words = Cut(handle, s, len);
  for (x = words; x->word; x++)
  {
    printf("%.*s\n", (int)x->len, x->word);
  }
  FreeWords(words);

  FreeJieba(handle);
}

int main(int argc, char **argv)
{
  CutDemo();
  CutForSearchDemo();
  // CutWithoutTagNameDemo();
  // ExtractDemo();
  // UserWordDemo();
  return 0;
}
