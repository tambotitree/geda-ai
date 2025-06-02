--- /dev/null
+++ b/Users/johnryan/Documents/GitHub/geda-ai/admin/README.md
@@ -0,0 +1,30 @@
+# Admin Scripts for gEDA-AI
+
+This directory contains administrative scripts used by the gEDA-AI build and installation process. These scripts are not typically run directly by end-users but are invoked by `make` targets or other build system components.
+
+## `geda-update-databases.sh`
+
+**Purpose:**
+
+The `geda-update-databases.sh` script is a custom solution designed to replace the direct dependency on system-wide XDG (X Desktop Group) database update utilities like `update-mime-database` and `update-desktop-database`.
+
+**Motivation:**
+
+1.  **Cross-Platform Consistency:** macOS does not have a standard XDG desktop or MIME database. Relying on system tools creates platform-specific installation issues. This script provides a unified (though potentially no-op on some platforms) mechanism.
2.  **User-Local Installs & No `sudo`:** By managing these updates through a custom script, `geda-ai` can be installed into user-local prefixes (e.g., `$HOME/geda`) without requiring `sudo` privileges for system-wide database updates. This ensures all necessary files, including icons and MIME types, can be managed within the installation prefix.
3.  **Self-Sufficiency:** Reduces external dependencies, making `geda-ai` more self-contained.
+
+**Invocation:**
+
+This script is invoked by `make install` rules. The build system (specifically, `m4/geda-desktop.m4` during the `configure` stage) sets up `UPDATE_MIME_DATABASE` and `UPDATE_DESKTOP_DATABASE` make variables to point to this script.
+
+It is called with two arguments:
+1.  `$1`: The type of update (`mime` or `desktop`).
+2.  `$2`: The target directory where the relevant files (e.g., `.xml` MIME files or `.desktop` files) have been installed.
+
+**Behavior:**
+
+*   **macOS:** Currently, this script is expected to perform a no-op or minimal local actions, as macOS does not use the XDG database system in the same way as Linux.
+*   **Linux (User-Local Prefix):** The script will attempt to handle database updates locally within the specified installation prefix. This might involve ensuring files are in the correct subdirectories (e.g., `$PREFIX/share/mime/packages`, `$PREFIX/share/applications`) so that they can be picked up by the desktop environment if `$PREFIX/share` is part of `XDG_DATA_DIRS`.
+*   **Windows:** (Future) Behavior for Windows will need to be defined if XDG-like integration is desired.
+
+This script is crucial for achieving a `sudo`-less `make install` and ensuring that gEDA-AI can run correctly from a user-defined prefix without interfering with or relying on system-wide configurations where avoidable or not applicable.

