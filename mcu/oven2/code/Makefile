#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := oven2

include $(IDF_PATH)/make/project.mk

WEB_SRC_DIR = $(shell pwd)/front
SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
$(eval $(call spiffs_create_partition_image,www,$(WEB_SRC_DIR)))
