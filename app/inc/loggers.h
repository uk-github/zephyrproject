#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>

#define LOG_LEVEL_DEBUG     (0)
#define LOG_LEVEL_INFO      (1)
#define LOG_LEVEL_WARN      (2)
#define LOG_LEVEL_ERROR     (3)
#define LOG_LEVEL_FATAL     (4)
#define LOG_OFF             (5)

#define LOG_LEVEL           (LOG_LEVEL_INFO)

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...)     printk("[D] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)      printk("[I] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)      printk("[W] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printk("[E] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)     printk("[F] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_DEBUG(fmt, ...)     do { } while (0)
#define LOG_INFO(fmt, ...)      printk("[I] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)      printk("[W] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printk("[E] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)     printk("[F] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_DEBUG(fmt, ...)     do { } while (0)
#define LOG_INFO(fmt, ...)      do { } while (0)
#define LOG_WARN(fmt, ...)      printk("[W] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)     printk("[E] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)     printk("[F] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_DEBUG(fmt, ...)     do { } while (0)
        #define LOG_INFO(fmt, ...)      do { } while (0)
#define LOG_WARN(fmt, ...)      do { } while (0)
#define LOG_ERROR(fmt, ...)     printk("[E] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)     printk("[F] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#elif LOG_LEVEL <= LOG_LEVEL_FATAL
#define LOG_DEBUG(fmt, ...)     do { } while (0)
#define LOG_INFO(fmt, ...)      do { } while (0)
#define LOG_WARN(fmt, ...)      do { } while (0)
#define LOG_ERROR(fmt, ...)     do { } while (0)
#define LOG_FATAL(fmt, ...)     printk("[F] %s: %s(%d): " fmt "\n", LOG_TAG,  __func__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)  do { } while (0)
#define LOG_INFO(fmt, ...)   do { } while (0)
#define LOG_WARN(fmt, ...)   do { } while (0)
#define LOG_ERROR(fmt, ...)  do { } while (0)
#define LOG_FATAL(fmt, ...)  do { } while (0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define BEGIN()     printk("[D] %s: %s(%d): BEGIN\n",   LOG_TAG,  __func__, __LINE__)
#define END()       printk("[D] %s: %s(%d): END\n",     LOG_TAG,  __func__, __LINE__)
#else
#define BEGIN()     do { } while (0)
#define END()       do { } while (0)
#endif 