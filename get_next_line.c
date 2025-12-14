#include "get_next_line.h"

static int create_list(t_list **list, int fd);
static int found_newline(t_list *list);
static int append(t_list **list, char *buf);
static t_list *find_last_node(t_list *list);
static char *get_line(t_list *list);
static int len_to_newline(t_list *list);
static void copy_str(t_list *list, char *str);
static int polish_list(t_list **list);
static void dealloc(t_list **list, t_list *clean_node, char *buf);

int main(void) {
  int fd;
  char *line;
  int lines;

  lines = 1;
  fd = open("text.txt", O_RDONLY);

  while ((line = get_next_line(fd))) {
    printf("%d->%s\n", lines++, line);
  }
}

char *get_next_line(int fd) {
  static t_list *list = NULL;
  char *next_line;

  if (fd < 0 || BUFFER_SIZE <= 0) {
    return NULL;
  }

  if (read(fd, NULL, 0) < 0) {
    dealloc(&list, NULL, NULL);
    return NULL;
  }

  if (create_list(&list, fd) < 0) {
    dealloc(&list, NULL, NULL);
    return NULL;
  }

  if (list == NULL) {
    return NULL;
  }

  next_line = get_line(list);
  if (next_line == NULL) {
    dealloc(&list, NULL, NULL);
    return NULL;
  }

  if (polish_list(&list) < 0) {
    free(next_line);
    dealloc(&list, NULL, NULL);
    return NULL;
  }

  return next_line;
}

static int create_list(t_list **list, int fd) {
  int char_read;
  char *buf;

  while (!found_newline(*list)) {
    buf = malloc(BUFFER_SIZE + 1);
    if (buf == NULL) {
      return -1;
    }

    char_read = read(fd, buf, BUFFER_SIZE);
    if (char_read < 0) {
      free(buf);
      return -1;
    }

    if (!char_read) {
      free(buf);
      return 0;
    }

    buf[char_read] = '\0';
    if (append(list, buf) < 0) {
      free(buf);
      return -1;
    }
  }
  return 0;
}

static int found_newline(t_list *list) {
  int i;

  if (list == NULL) {
    return 0;
  }

  while (list) {
    i = 0;
    while (list->str_buf[i]) {
      if (list->str_buf[i] == '\n') {
        return 1;
      }
      i++;
    }
    list = list->next;
  }
  return 0;
}

static int append(t_list **list, char *buf) {
  t_list *new_node;
  t_list *last_node;

  last_node = find_last_node(*list);
  new_node = malloc(sizeof(t_list));

  if (new_node == NULL) {
    return -1;
  }

  if (last_node == NULL) {
    *list = new_node;
  } else {
    last_node->next = new_node;
  }
  new_node->str_buf = buf;
  new_node->next = NULL;
  return 0;
}

static t_list *find_last_node(t_list *list) {
  if (list == NULL) {
    return NULL;
  }

  while (list->next) {
    list = list->next;
  }

  return list;
}

static char *get_line(t_list *list) {
  int str_len;
  char *next_str;

  if (list == NULL) {
    return NULL;
  }

  str_len = len_to_newline(list);
  next_str = malloc(str_len + 1);

  if (next_str == NULL) {
    return NULL;
  }

  copy_str(list, next_str);
  return next_str;
}

static int len_to_newline(t_list *list) {
  int i;
  int len;

  if (list == NULL) {
    return 0;
  }

  len = 0;
  while (list) {
    i = 0;
    while (list->str_buf[i]) {
      if (list->str_buf[i] == '\n') {
        ++len;
        return len;
      }
      ++i;
      ++len;
    }
    list = list->next;
  }
  return len;
}

static void copy_str(t_list *list, char *str) {
  int i;
  int k;

  if (list == NULL || str == NULL) {
    return;
  }

  k = 0;
  while (list) {
    i = 0;
    while (list->str_buf[i]) {
      if (list->str_buf[i] == '\n') {
        str[k++] = '\n';
        str[k] = '\0';
        return;
      }
      str[k++] = list->str_buf[i++];
    }
    list = list->next;
  }
  str[k] = '\0';
}

static int polish_list(t_list **list) {
  t_list *last_node;
  t_list *clean_node;
  int i;
  int k;
  char *buf;

  buf = malloc(BUFFER_SIZE + 1);
  clean_node = malloc(sizeof(t_list));

  if (buf == NULL || clean_node == NULL) {
    free(buf);
    free(clean_node);
    return -1;
  }

  last_node = find_last_node(*list);
  i = 0;
  k = 0;

  while (last_node->str_buf[i] && last_node->str_buf[i] != '\n') {
    ++i;
  }
  while (last_node->str_buf[i] && last_node->str_buf[++i]) {
    buf[k++] = last_node->str_buf[i];
  }

  buf[k] = '\0';
  clean_node->str_buf = buf;
  clean_node->next = NULL;
  dealloc(list, clean_node, buf);
  return 0;
}

static void dealloc(t_list **list, t_list *clean_node, char *buf) {
  t_list *tmp;

  if (list == NULL || *list == NULL) {
    return;
  }

  while (*list) {
    tmp = (*list)->next;
    free((*list)->str_buf);
    free(*list);
    *list = tmp;
  }

  *list = NULL;

  if (clean_node == NULL) {
    return;
  }

  if (clean_node->str_buf[0]) {
    *list = clean_node;
  } else {
    free(buf);
    free(clean_node);
  }
}
