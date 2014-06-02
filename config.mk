# Components of a version number:
#   Board: 1st Redwood board is always L (for Laminator)
#   Series: Which board in the series Redwood boards implemented
#   Product: Major change or D5 project phase change
#   Maint: Minor changes and bug fixes
#   Deviation: Follows Jeff Davie's new plan for deviation numbers:
#      00000-00999 = Build number, increasing with every build
#      01000-04999 = Custom firmware number
#      05000-09999 = Deviation and buf-fix release number
#      20000-65534 = Special builds for specific purposes
#            65535 = Developer-only builds
#
BOARD     = L
SERIES    = 1
PRODUCT   = 1
MAINT     = 2
DEVIATION = 2

#
#Increment the SPECIAL variable for each special build, and document what the build is for below.
#Build with "make special"
#Special builds used:
# 10000: 
SPECIAL   = 0
