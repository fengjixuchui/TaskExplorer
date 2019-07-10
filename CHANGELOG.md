# Changelog
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).


## [0.3] - 2019-07-09
### Added
- tokens tab with advanced infos
- improved handle window
-- show job info window 
-- show token info window 
-- show task info window
-- open file lokation
-- open registry key
-- read/write section memory
-- type filter now enumerates all types
- added size info to section type handle

### Changed
- rewoked sid to username resolution now using a worker thread to improve performance
- CWinProcess does nto longer handle sid_user/token informations all is doem by CWinToken instead

### Fixed
- fixed issue with the first graph text not being displayed
- fixed an issue causing the client to wait for 10 sec on shutdown



## [0.2] - 2019-07-05
### Added
- memory tab, with options to dump the memory, free it or change access permissions
- advanced memory editor window
-- forked qhexedit2 https://github.com/DavidXanatos/qhexedit to ad missing functionality, edit, lock mode, etc...
-- added a QHexEditor class to qhexedit implementing a generic hex editor dialog with options and search capability

### Changed
- I/O stats does not longer show ETW values when the is not monitoring ETW events

### Fixed
- fixed Uptime column not being refreshed



## [0.1] - 2019-06-30
### Added
- service property window, including all pages form the extended services plugin
- cpu and memory usage in tray icon
- option to start elevanted without an UAC prompt
- auto run using windows registry
- build x86 binaries
- add option to create a process dump
- afility to run a program with the tocken of an other program (run as this user)
- services tab showing services hosted by the selcted process
- add go to service key
- type filter to handles
- added missing handle actions
- add window properties details area

### Changed 
- paged memory usage is now extracted form pagefile informtions
- monitoring of ETW events can be disabled and re enabled in tool menu
- handle property detail are no a tree widget

### Fixed
- crash when querying samba datarate and getting null
- run as feature forks now
- fixed issue with CoInitialize



## [0.0.9] - 2019-06-24
### Added
- general system tab
- process and system stats
- job tab
- add process pid picker dialog
- add ras/vpn graph
- process and threads are listed for 5 sec after termination
- grid to all lists
- service column to process tree
- list colloring
- context menu to services view
- tools menu
- scm permissions
- processor affinity dialog
- organized columns menu for the process tree in sub menus
- added option to restart elevated
- added graphs to process tree, CPU, Memory, IO/DiskIO, Network
- linux style cpu usage i.e. 1 core = 100% so > 1 core -> > 100%
- create service dialog
- taskexplorer can now be started as service and listen for commands
- option to start programs as TrustedInstaller without using a service
- run as dialog

### Changed 
- graphs can now be resized with a splitter
- improved process tree sorting behavioure to be more like in process hacker
- samba stats now using NetStatisticsGet instead of speculating on ETW events
- improved global network traffic logging now using GetIfTable2 instead of ETW events
- improved MMapIO display now it works as expected and disk IO got its own graph
- all files list now works for non enevated users
- driver tab now uses NtQuerySystemInformation(SystemModuleInformation to enumerate drivers

### Fixed
- memory leak when running without unelevated and vieving all files list


## [0.0.8] - 2019-06-17
### Added
- window graph 
- window column in process tree
- adde dtreee to window tab
- environment vartiable view
- context menu to environment vartiable 
- all thread model columns
- all process model columns
- context menu to sockets view
- context menu to handles view
- context menu to modules view
- CAbstractTask Class to abstract threads and processes
- linked split tree selection
- context menu to threads view
- context menu to process tree
- double click on a process opens the Info panel in a new window
- context menu to windows view
- permissions dialog to threads processes and handles
- saving window position spliter positions column selection and more
- added sys tray icon
- general task tab 


### Changed

### Fixed
- failed to remove old modules
- fixed windows tree not opening

## [0.0.7] - 2019-06-10
### Added
- network stats
- disk stats
- CPU Usage graph
- Memory Usage graph
- FileIO and DIskIO graph
- Network graph
- Samba graph
- GUI/USER Object graph
- Handles graph

### Changed
- reworked the process tree handling for better performance
- added abstract tree model
- added abstract list model

### Fixed
- endpoint swap of incomming udp packets
- fixed memory leak during all files enumeration

## [0.0.6] - 2019-06-08
### Added
- module view

## [0.0.5] - 2019-06-04
### Added
- service list
- driver list

## [0.0.4] - 2019-06-02
### Added
- Thread list / Stack trace
- added All Files view

## [0.0.3] - 2019-06-01
### Added
- File / Handle list
- Disk/Network stat event listener


## [0.0.2] - 2019-05-31
### Added
- ProcessHacker Library integration
- Global and per process Listing of open sockets/connections

### Changed
- Improved Process Tree performance

## [0.0.1] - 2019-05-30
### Added
- ProcessHacker Library integration