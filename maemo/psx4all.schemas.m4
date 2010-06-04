<?xml version="1.0" encoding="utf-8"?>
define(`choose', `$2')dnl
define(`N_', `$*')dnl
<gconfschemafile>
  <schemalist>
    <schema>
      <key>/schemas/apps/maemo/psx4all/display-framerate</key>
      <applyto>/apps/maemo/psx4all/display-framerate</applyto>
      <owner>psx4all</owner>
      <type>bool</type>
      <default>false</default>
      <locale name="C">
        <short>Display framerate</short>
        <long>
          Display a framerate counter in the lower left corner.
        </long>
      </locale>
    </schema>
    <schema>
      <key>/schemas/apps/maemo/psx4all/frameskip</key>
      <applyto>/apps/maemo/psx4all/frameskip</applyto>
      <owner>psx4all</owner>
      <type>int</type>
      <default>0</default>
      <locale name="C">
        <short>Frameskip</short>
        <long>
          Skip this many frames after rendering one frame (or 0 for auto).
        </long>
      </locale>
    </schema>
    <schema>
     <key>/schemas/apps/maemo/psx4all/sound</key>
      <applyto>/apps/maemo/psx4all/sound</applyto>
      <owner>psx4all</owner>
      <type>bool</type>
      <default>true</default>
      <locale name="C">
        <short>Enable sound</short>
        <long>
          Enable emulation and output of sound.
        </long>
      </locale>
    </schema>
    <schema>
     <key>/schemas/apps/maemo/psx4all/player1/keyboard/enable</key>
      <applyto>/apps/maemo/psx4all/player1/keyboard/enable</applyto>
      <owner>psx4all</owner>
      <type>bool</type>
      <default>true</default>
      <locale name="C">
        <short>Player 1 keyboard</short>
        <long>
          Enable key mappings for player 1.
        </long>
      </locale>
    </schema>
dnl Player 1 keybindings
define(`HELP', `')dnl
define(`BUTTON', `dnl
    <schema>
     <key>/schemas/apps/maemo/psx4all/player1/keyboard/$2</key>
      <applyto>/apps/maemo/psx4all/player1/keyboard/$2</applyto>
      <owner>psx4all</owner>
      <type>int</type>
      <default>choose($4,$5)</default>
      <locale name="C">
        <short>$1 button</short>
      </locale>
    </schema>
dnl')dnl
define(`ACTION', `dnl
    <schema>
     <key>/schemas/apps/maemo/psx4all/player1/keyboard/$2</key>
      <applyto>/apps/maemo/psx4all/player1/keyboard/$2</applyto>
      <owner>psx4all</owner>
      <type>int</type>
      <default>choose($4,$5)</default>
      <locale name="C">
        <short>$1 action</short>
      </locale>
    </schema>
dnl')dnl
define(`LAST', `')dnl
include(buttons.inc)dnl
undefine(`HELP')dnl
undefine(`BUTTON')dnl
undefine(`ACTION')dnl
undefine(`LAST')dnl
    <schema>
     <key>/schemas/apps/maemo/psx4all/player2/keyboard/enable</key>
      <applyto>/apps/maemo/psx4all/player2/keyboard/enable</applyto>
      <owner>psx4all</owner>
      <type>bool</type>
      <default>false</default>
      <locale name="C">
        <short>Player 2 keyboard</short>
        <long>
          Enable key mappings for player 2.
        </long>
      </locale>
    </schema>
dnl Player 2 keybindings
define(`HELP', `')dnl
define(`BUTTON', `dnl
    <schema>
     <key>/schemas/apps/maemo/psx4all/player2/keyboard/$2</key>
      <applyto>/apps/maemo/psx4all/player2/keyboard/$2</applyto>
      <owner>psx4all</owner>
      <type>int</type>
      <default>0</default>
      <locale name="C">
        <short>$1 button</short>
      </locale>
    </schema>
dnl')dnl
define(`ACTION', `')dnl
define(`LAST', `')dnl
include(buttons.inc)
undefine(`HELP')dnl
undefine(`BUTTON')dnl
undefine(`ACTION')dnl
undefine(`LAST')dnl
  </schemalist>
</gconfschemafile>
