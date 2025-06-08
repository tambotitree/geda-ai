#!/bin/sh
# geda-update-databases.sh
#
set -eu # Exit immediately if a command exits with a non-zero status or if an unset variable is used.
#
# Updates gEDA-AI specific application databases (MIME types, desktop entries)
# by processing files within the installation prefix and updating an internal cache.
# This script provides workalike behavior for update-mime-database and
# update-desktop-database for gEDA-AI's internal needs, facilitating cross-platform
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
  echo "TARGET_DIR is the gEDA-AI specific data directory for the given TYPE."
  echo "           It is expected to be a path like '\$(datadir)/geda-ai/mime'"
  echo "           (for MIME types) or '\$(datadir)/geda-ai/applications'"
  echo "           (for desktop entries)."
  echo "           For 'mime', TARGET_DIR should contain a 'packages' subdirectory with XML source files."
  echo "           For 'desktop', TARGET_DIR should directly contain .desktop source files."
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
RAW_TARGET_DIR="$2" # e.g., $(DESTDIR)$(prefix)/share/geda/mime or $(DESTDIR)$(prefix)/share/geda-ai/applications

echo "gEDA custom database update script called:"
echo "Type: ${TYPE}"
echo "Raw Target Directory (from Makefile): ${RAW_TARGET_DIR}"

# Determine OS
# OS_TYPE=$(uname) # Currently unused, can be removed if not needed for future OS-specific logic.

# Determine the effective gEDA-AI share directory and internal database directory.
# RAW_TARGET_DIR is e.g. $(DESTDIR)$(prefix)/share/geda-ai/mime or $(DESTDIR)$(prefix)/share/geda/applications
# The gEDA-AI application root directory (e.g., $(DESTDIR)$(prefix)/share/geda-ai)
# is expected to be the parent of RAW_TARGET_DIR.
GEDA_AI_APP_ROOT=$(dirname "${RAW_TARGET_DIR}")

# set our expectations to be ${GEDADATADIR} . because who knows what kind of legacy troble we could get into?
# Flexible check: look for geda/share in path
case "${RAW_TARGET_DIR}" in
  */geda/share/*)
    echo "Detected valid geda directory layout."
    ;;
  *)
    echo "Error: RAW_TARGET_DIR '${RAW_TARGET_DIR}' does not appear to be under a 'geda/share/' hierarchy." >&2
    exit 1
    ;;
esac

GEDA_AI_INTERNAL_DB_DIR="${GEDA_AI_APP_ROOT}/db"
mkdir -p "${GEDA_AI_INTERNAL_DB_DIR}" || { echo "Error: Could not create internal database directory ${GEDA_AI_INTERNAL_DB_DIR}" >&2; exit 1; }

ACTUAL_SOURCE_DATA_DIR=""

case "${TYPE}" in
  mime)
    echo "Updating gEDA-AI internal MIME database..."
    # XML files are in a 'packages' subdirectory of the raw target dir for mime
    ACTUAL_SOURCE_DATA_DIR="${RAW_TARGET_DIR}/packages"
    MIME_CACHE_FILE="${GEDA_AI_INTERNAL_DB_DIR}/mime.cache"

    echo "Scanning ${ACTUAL_SOURCE_DATA_DIR} for gEDA-AI MIME type definitions (*.xml)..."
    if [ ! -d "${ACTUAL_SOURCE_DATA_DIR}" ] || [ -z "$(ls -A "${ACTUAL_SOURCE_DATA_DIR}"/*.xml 2>/dev/null)" ]; then
        echo "No gEDA-AI MIME XML files found in ${ACTUAL_SOURCE_DATA_DIR} or directory does not exist."
    else
        find "${ACTUAL_SOURCE_DATA_DIR}" -name "*.xml" -print
        echo "Found MIME XML files. (Placeholder: actual parsing and cache generation would occur here)."
    fi
    touch "${MIME_CACHE_FILE}" || { echo "Error: Could not touch MIME cache file ${MIME_CACHE_FILE}" >&2; exit 1; }
    echo "gEDA-AI internal MIME database updated (placeholder: ${MIME_CACHE_FILE})."
    ;;
  desktop)
    echo "Updating gEDA-AI internal Desktop Entry database..."
    # Desktop files are directly in the raw target dir for desktop
    ACTUAL_SOURCE_DATA_DIR="${RAW_TARGET_DIR}"
    DESKTOP_CACHE_FILE="${GEDA_AI_INTERNAL_DB_DIR}/desktop.cache"

    echo "Scanning ${ACTUAL_SOURCE_DATA_DIR} for gEDA-AI desktop entries (*.desktop)..."
    if [ ! -d "${ACTUAL_SOURCE_DATA_DIR}" ] || [ -z "$(ls -A "${ACTUAL_SOURCE_DATA_DIR}"/*.desktop 2>/dev/null)" ]; then
        echo "No gEDA-AI .desktop files found in ${ACTUAL_SOURCE_DATA_DIR} or directory does not exist."
    else
        find "${ACTUAL_SOURCE_DATA_DIR}" -name "*.desktop" -print
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
