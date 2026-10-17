#!/bin/bash

# Backup first
cp actions.c actions_old.c

# Strip macros, expand
awk '
/^DEFINE_ACTION/ {
    gsub("DEFINE_ACTION \\(", "", $0);
    sub("\\)$", "", $0);
    split($0, parts, ",");
    cid = parts[1];
    id = parts[2];
    icon = parts[3];
    name = parts[4];
    label = parts[5];
    menu_label = parts[6];
    tooltip = parts[7];
    type = parts[8];

    # Print global declaration
    printf("GschemAction *action_%s;\n\n", cid);

    # Print callback
    printf("void action_callback_%s(GschemAction *action, GschemToplevel *w_current)\n{\n", cid);
    in_callback = 1;
    next;
}

in_callback && /^\}/ {
    print $0 "\n";
    # Print registration
    printf("action_%s = gschem_action_register(%s, %s, %s, %s, %s, %s, %s, action_callback_%s);\n\n",
           cid, id, icon, name, label, menu_label, tooltip, type, cid);
    in_callback = 0;
    next;
}

in_callback {
    print;
    next;
}

!in_callback {
    print;
}
' actions_old.c > actions.c

echo "✅ Generated new actions.c from actions_old.c"
