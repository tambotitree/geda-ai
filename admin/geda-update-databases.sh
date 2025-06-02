#!/bin/sh
# geda-update-databases.sh
#
# Updates gEDA-AI specific application databases (MIME types, desktop entries)
# by processing files within the installation prefix and updating an internal cache.
# This script is intended to be OS-agnostic for its core functionality.

usage() {
  echo "Usage: $0 TYPE TARGET_DIR"
  echo "       $0 --help"
  echo ""
  echo "Updates gEDA-AI specific application databases."
  echo ""
  echo "TYPE can be:"
  echo "  mime      - Update gEDA-AI internal MIME type information cache."
  echo "  desktop   - Update gEDA-AI internal desktop application entry cache."
  echo ""
  echo "TARGET_DIR is the source directory for the update, typically within the"
  echo "installation prefix (e.g., \$(DESTDIR)\$(prefix)/share/geda-ai/mime/packages for MIME,"
  echo "or \$(DESTDIR)\$(prefix)/share/geda-ai/applications for desktop entries)."
  exit 1
}

if [ "$1" = "--help" ]; then
  usage
fi

if [ $# -ne 2 ]; then
  echo "Error: Incorrect number of arguments." >&2
  usage
fi

TYPE="$1"
TARGET_DIR="$2"

echo "gEDA custom database update script called:"
echo "Type: ${TYPE}"
echo "Source Directory: ${TARGET_DIR}"

# Determine OS
OS_TYPE=$(uname)

# Determine the effective gEDA-AI share directory and internal database directory.
# TARGET_DIR is expected to be like $(DESTDIR)$(prefix)/share/geda-ai/mime/packages or $(DESTDIR)$(prefix)/share/geda-ai/applications
GEDA_AI_MIME_OR_APPS_DIR=$(dirname "${TARGET_DIR}") # e.g. $(DESTDIR)$(prefix)/share/geda-ai/mime
PREFIX_SHARE_GEDA_AI=$(dirname "${GEDA_AI_MIME_OR_APPS_DIR}") # e.g. $(DESTDIR)$(prefix)/share/geda-ai

if [ "$(basename "${PREFIX_SHARE_GEDA_AI}")" != "geda-ai" ]; then
    echo "Error: TARGET_DIR '${TARGET_DIR}' does not appear to be within a 'geda-ai' subdirectory of a share path (e.g., .../share/geda-ai/...)." >&2
    exit 1
fi

GEDA_AI_INTERNAL_DB_DIR="${PREFIX_SHARE_GEDA_AI}/db"
mkdir -p "${GEDA_AI_INTERNAL_DB_DIR}" || { echo "Error: Could not create internal database directory ${GEDA_AI_INTERNAL_DB_DIR}" >&2; exit 1; }

case "${TYPE}" in
  mime)
    echo "Updating gEDA-AI internal MIME database..."
    MIME_SOURCE_DIR="${TARGET_DIR}" # This should be $PREFIX/share/geda-ai/mime/packages
    MIME_CACHE_FILE="${GEDA_AI_INTERNAL_DB_DIR}/mime.cache"

    echo "Scanning ${MIME_SOURCE_DIR} for gEDA-AI MIME type definitions (*.xml)..."
    if [ ! -d "${MIME_SOURCE_DIR}" ] || [ -z "$(ls -A "${MIME_SOURCE_DIR}"/*.xml 2>/dev/null)" ]; then
        echo "No gEDA-AI MIME XML files found in ${MIME_SOURCE_DIR} or directory does not exist."
    else
        find "${MIME_SOURCE_DIR}" -name "*.xml" -print
        echo "Found MIME XML files. (Placeholder: actual parsing and cache generation would occur here)."
    fi
    touch "${MIME_CACHE_FILE}" || { echo "Error: Could not touch MIME cache file ${MIME_CACHE_FILE}" >&2; exit 1; }
    echo "gEDA-AI internal MIME database updated (placeholder: ${MIME_CACHE_FILE})."
    ;;
  desktop)
    echo "Updating gEDA-AI internal Desktop Entry database..."
    DESKTOP_SOURCE_DIR="${TARGET_DIR}" # This should be $PREFIX/share/geda-ai/applications
    DESKTOP_CACHE_FILE="${GEDA_AI_INTERNAL_DB_DIR}/desktop.cache"

    echo "Scanning ${DESKTOP_SOURCE_DIR} for gEDA-AI desktop entries (*.desktop)..."
    if [ ! -d "${DESKTOP_SOURCE_DIR}" ] || [ -z "$(ls -A "${DESKTOP_SOURCE_DIR}"/*.desktop 2>/dev/null)" ]; then
        echo "No gEDA-AI .desktop files found in ${DESKTOP_SOURCE_DIR} or directory does not exist."
    else
        find "${DESKTOP_SOURCE_DIR}" -name "*.desktop" -print
        echo "Found .desktop files. (Placeholder: actual parsing and cache generation would occur here)."
    fi
    touch "${DESKTOP_CACHE_FILE}" || { echo "Error: Could not touch desktop cache file ${DESKTOP_CACHE_FILE}" >&2; exit 1; }
    echo "gEDA-AI internal Desktop Entry database updated (placeholder: ${DESKTOP_CACHE_FILE})."
    ;;
  *)
    echo "Error: Unknown TYPE '${TYPE}'" >&2
    usage
    ;;
esac

exit 0
fi

exit 0
