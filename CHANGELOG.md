<a name="4.10.12"></a>
### 4.10.12 (2019-06-25)


#### Bug Fixes

* **ldap:**  login not enabled after joining domain ([0c645d91](https://github.com/linuxdeepin/dde-control-center/commit/0c645d91055b488d1d6bde0dd7f732bb7e3be2ff))
* **nav:**  missing disable systeminfo module ([02511c4a](https://github.com/linuxdeepin/dde-control-center/commit/02511c4ae7f10887dceb503f4d6a7d6db745d919))
* **sync:**  typo DISALBE -> DISABLE ([95356783](https://github.com/linuxdeepin/dde-control-center/commit/95356783358dfa0581122d9da50eb6b6c2b649d8))

#### Others

*   missing build macro ([b8681869](https://github.com/linuxdeepin/dde-control-center/commit/b8681869731642764603db33f748566ff82cdac0))
* **update:**  remove network check ([b52f5267](https://github.com/linuxdeepin/dde-control-center/commit/b52f526753bdd9822934e13d346d70ceb4f0bfd8))

#### Features

* **navbar:**  avoid tip grab focus ([d4251454](https://github.com/linuxdeepin/dde-control-center/commit/d4251454f512a8f0ab0be9aa444b4c24b8124c65))



<a name="4.10.11"></a>
### 4.10.11 (2019-06-14)


#### Bug Fixes

* **accounts:**  cannot modify user password ([0c68cc3b](https://github.com/linuxdeepin/dde-control-center/commit/0c68cc3bd9e2d4b7f161322094c3da734fc5818f))
* **network:**  Abnormal interface of hostpot ([551d13e9](https://github.com/linuxdeepin/dde-control-center/commit/551d13e904c232fae0e193c846f948208fd44da0))



<a name="4.10.10"></a>
### 4.10.10 (2019-06-13)


#### Others

*   update license ([765fc346](https://github.com/linuxdeepin/dde-control-center/commit/765fc346bf2107bd33af55cb46585878187ae375))



<a name="4.10.9"></a>
### 4.10.9 (2019-06-11)


#### Features

* **Sync:**  add config files ([a9415192](https://github.com/linuxdeepin/dde-control-center/commit/a941519291894c2f3fa43248b34cd8e5305f002f))

#### Bug Fixes

*   incorrect layout of various pages. ([c9360985](https://github.com/linuxdeepin/dde-control-center/commit/c9360985d99ee4502c5dd7e04dd4cd19df7cfe23))
*   set word wrap on switch widget construction. ([bee7372e](https://github.com/linuxdeepin/dde-control-center/commit/bee7372e84deec36c4d83a5f2c1ec1d00805e79f))
* **sync:**
  *  syncstate widget was truncated the first time started ([9c814dd9](https://github.com/linuxdeepin/dde-control-center/commit/9c814dd979feef9d08fe74649fd64b0500466741))
  *  last sync time always not show ([e29491fe](https://github.com/linuxdeepin/dde-control-center/commit/e29491fecb1f5a95dc22fb979ee4035b722aea0c))
* **update:**  missing smarttips spacing ([f9e5ff78](https://github.com/linuxdeepin/dde-control-center/commit/f9e5ff7812610aeafcde2413e2d3d636940a1724))



<a name="4.10.8"></a>
### 4.10.8 (2019-06-04)


#### Bug Fixes

*   switch widget caption text overflows. (#212) ([a16647b3](https://github.com/linuxdeepin/dde-control-center/commit/a16647b30591978721fdc5d7f61345970b7a60e9))
*   dbus method 'rect' returns wrong results after screen rotation. ([6fe8c17d](https://github.com/linuxdeepin/dde-control-center/commit/6fe8c17dc4386dc9f0093a6121c462c0c445ed31))
* **dbus:**  check windowhandle ([25f0ff7d](https://github.com/linuxdeepin/dde-control-center/commit/25f0ff7dd7e931991d3695feaab0551db2a5477d))

#### Features

* **sync:**
  *  add region check ([c519e4b4](https://github.com/linuxdeepin/dde-control-center/commit/c519e4b4e8b5402a22b65e8b23e1feb7fdb558e8))
  *  disable cloud sync for professional platform ([1e6c4dee](https://github.com/linuxdeepin/dde-control-center/commit/1e6c4deebfc0d099ded972ef8b0042706b9981c6))
  *  upload new syncing icon ([f5eed42a](https://github.com/linuxdeepin/dde-control-center/commit/f5eed42a0abda087abafe9f09f94085d0a0a56bb))
  *  check sync daemon isvalid ([9da7475a](https://github.com/linuxdeepin/dde-control-center/commit/9da7475a0352136666d1612e0618fdb33eac597b))
  *  set syncing when lastsynctime is zero ([1f89546d](https://github.com/linuxdeepin/dde-control-center/commit/1f89546de52aac1b369134249d06e2492e2fc764))
  *  add sync module state changed ([ae369562](https://github.com/linuxdeepin/dde-control-center/commit/ae3695624d96223b75751f48a6001fbeee9bb4e7))
  *  add logout button ([550652ca](https://github.com/linuxdeepin/dde-control-center/commit/550652ca260442f26a17194e672249792f8c4ce8))
  *  add sync state icon ([88eba3c8](https://github.com/linuxdeepin/dde-control-center/commit/88eba3c8c58d12858f5c60ea94e7e9cfc923280b))
  *  add theme power corner SyncType ([eff81f96](https://github.com/linuxdeepin/dde-control-center/commit/eff81f96c70324838df2a4f0f3f5dcab449d49b9))
  *  add sync module list ([d02993d5](https://github.com/linuxdeepin/dde-control-center/commit/d02993d566862dc8d28150f6f818029bb09c651a))
  *  add sync state widget ([aa5a5bdf](https://github.com/linuxdeepin/dde-control-center/commit/aa5a5bdf71f1ce237e64019b2a497088c789b5f7))
  *  show user info when user logind ([7d36e283](https://github.com/linuxdeepin/dde-control-center/commit/7d36e283c8fc767efebd44b748fd84adedb1f9b7))
  *  init sync module ([9a8ae08d](https://github.com/linuxdeepin/dde-control-center/commit/9a8ae08d660cf9ec5d5e85af60207ed31aec3218))



<a name="4.10.7"></a>
### 4.10.7 (2019-06-01)


#### Bug Fixes

*   long processor name screws up layout. ([722a95ef](https://github.com/linuxdeepin/dde-control-center/commit/722a95efc9e4d85fe83683ae88927eaaeb4436b6))
* **fprintd:**  refresh device when enter user detail page again ([b912e969](https://github.com/linuxdeepin/dde-control-center/commit/b912e969fea91c3c1f1baf6f5b5ea9ac077304ff))



<a name="4.10.6"></a>
### 4.10.6 (2019-05-29)


#### Bug Fixes

* **accounts:**  not hide error tip ([c4818bc7](https://github.com/linuxdeepin/dde-control-center/commit/c4818bc7dd9c60415f2fbaf0c2a2bf4383e40310))
* **sw:**  missing mieee ([048511fd](https://github.com/linuxdeepin/dde-control-center/commit/048511fdc85714d13cb77a6ee2d12269a2c3c5f3))



<a name="4.10.5"></a>
### 4.10.5 (2019-05-23)


#### Bug Fixes

*   missing polkit policy file ([0021142d](https://github.com/linuxdeepin/dde-control-center/commit/0021142d1740f915f0588fd128449bca3544a993))
* **display:**  Adjust the resolution when merging the screen, destroy the merge ([b7136579](https://github.com/linuxdeepin/dde-control-center/commit/b71365797b67937113bb40b2d43db523275938cd))

#### Features

* **shortcut:**  add restore defaults button ([cebb30a2](https://github.com/linuxdeepin/dde-control-center/commit/cebb30a221d5421bcb3ea991254bdb8cbfd95677))



<a name="4.10.4"></a>
### 4.10.4 (2019-05-13)


#### Bug Fixes

* **Sound:**  typo Logout ([bf5325a2](https://github.com/linuxdeepin/dde-control-center/commit/bf5325a2348f24ce9dc98725ca95b0b2a7fe8499))



<a name="4.10.3"></a>
### 4.10.3 (2019-05-10)


#### Bug Fixes

*   Update notifications are not hidden ([6193a484](https://github.com/linuxdeepin/dde-control-center/commit/6193a484dceb52ec827be6423994d95682078ef3))
* **network:**  refresh loading icon not align center ([6840c8bc](https://github.com/linuxdeepin/dde-control-center/commit/6840c8bc3b0aee39a0c448273533dd2164e5b7cb))



<a name="4.10.2"></a>
## 4.10.2 (2019-04-25)


#### Bug Fixes

*   crash when startup in terminal ([f460f886](https://github.com/linuxdeepin/dde-control-center/commit/f460f886222e988a7b6fc32615adf76356eb1968))



<a name="4.10.1"></a>
### 4.10.1 (2019-04-25)




<a name=""></a>
##  4.10.0 (2019-04-19)


#### Bug Fixes

*   rotate dialog postion is wrong in 2D mode ([b0351d7c](https://github.com/linuxdeepin/dde-control-center/commit/b0351d7c56b4f5f373c6c46f8fcdf9b78a19040b))
*   ipv4 subnet mask valid checkup ([8aeede16](https://github.com/linuxdeepin/dde-control-center/commit/8aeede161e7681d81ebb910fd8b5e839714eb662))
*   wireless activating indicator not align in vertical ([42e88304](https://github.com/linuxdeepin/dde-control-center/commit/42e88304b888669e05561162e551bb24f8e30b94))
* **Display:**  did not check the multi scale ratio ([0434bfd2](https://github.com/linuxdeepin/dde-control-center/commit/0434bfd2183e180c199429e95650f153fd209b2f))



<a name="4.9.13"></a>
## 4.9.13 (2019-04-15)


#### Bug Fixes

*   wifi list sort algorithm ([dfe814a4](https://github.com/linuxdeepin/dde-control-center/commit/dfe814a4f7928a3d2b71edeb058272f0a0d3f3de))
* **miracast:**  wrong display nodevice item ([7eba25cd](https://github.com/linuxdeepin/dde-control-center/commit/7eba25cd83d1a5fe5485f446a2328b88b6bdfcd4))
* **sound:**  missing page title ([f33968a0](https://github.com/linuxdeepin/dde-control-center/commit/f33968a0d3793cfeb3078f6974d02ae00de427a5))



<a name="4.9.12"></a>
## 4.9.12 (2019-04-10)


#### Bug Fixes

* **display:**  exit rotate disloag after press Esc key ([ab023b91](https://github.com/linuxdeepin/dde-control-center/commit/ab023b915456ef97073c96fc5648b9ce9d33ba95))



<a name="4.9.11"></a>
## 4.9.11 (2019-04-10)




<a name="4.9.10"></a>
## 4.9.10 (2019-04-09)


#### Bug Fixes

*   multiple screen can not show ([2e976baa](https://github.com/linuxdeepin/dde-control-center/commit/2e976baa3a51a06925f9fc8f499bf123a64ec73b))



<a name="4.9.9.4"></a>
## 4.9.9.4 (2019-04-08)


#### Features

* **Display:**  use a profile to control multi-screen scale ratio ([2360bca9](https://github.com/linuxdeepin/dde-control-center/commit/2360bca986ffff1ed128b5f652c4fffb2bb9f60e))
* **Sound:**  hide some sound effect when not laptop ([4840656a](https://github.com/linuxdeepin/dde-control-center/commit/4840656ac8c4bdd5f748fb055613bf4979337961))

#### Bug Fixes

*   main window geometry is wrong ([2f0276fa](https://github.com/linuxdeepin/dde-control-center/commit/2f0276fa3e32485a60eb97fa0cfa7aa392623675))
* **Sound:**  skip not need values ([f8774697](https://github.com/linuxdeepin/dde-control-center/commit/f877469731992d75a15a78b7ba1ac2494fc681ca))
* **arm:**  disable power module ([3d819203](https://github.com/linuxdeepin/dde-control-center/commit/3d8192031f577925d2571d8d9889f23383869489))



<a name="4.9.9.2"></a>
## 4.9.9.2 (2019-03-28)




<a name="4.9.9.1"></a>
## 4.9.9.1 (2019-03-27)


#### Bug Fixes

*   hide option for individual scaling unless extend or custom configuration is in use. ([71bdf68a](https://github.com/linuxdeepin/dde-control-center/commit/71bdf68a55456bab3791886b21e1e5a9c57e63cd))



<a name="4.9.9"></a>
## 4.9.9 (2019-03-26)


#### Bug Fixes

*   keep the lenght of ssid among 1-32 ([ffbf5dae](https://github.com/linuxdeepin/dde-control-center/commit/ffbf5dae08742ee8ee474b917816166a3a3c471e))
*   compile error caused by dtk ([11daa363](https://github.com/linuxdeepin/dde-control-center/commit/11daa363ef55bda720f1d9cf4fbeaf4936aaf9f4))
*   translate failed in searchinput widget ([c3c8192e](https://github.com/linuxdeepin/dde-control-center/commit/c3c8192e826c36da82d7049b99e7e45d7cf651ad))
* **update:**  reboot-reminder-dialog install path error ([b11ac9aa](https://github.com/linuxdeepin/dde-control-center/commit/b11ac9aa324e5704dc7aa669332c2b63d918b6b0))

#### Features

*   initial work on scaling monitors individually ([6b17e883](https://github.com/linuxdeepin/dde-control-center/commit/6b17e8833e714d00814ec3a8f2148bef465d55a8))



<a name="4.9.8"></a>
### 4.9.8 (2019-03-19)


#### Bug Fixes

*   translate failed in defapp module ([51195d6f](https://github.com/linuxdeepin/dde-control-center/commit/51195d6f93819dea1c2dd6818456df972496da13))
* **power:**  remove battery in DISABLE_POWERSAVE macro ([d57a039d](https://github.com/linuxdeepin/dde-control-center/commit/d57a039de020ffa2e517ea7f1be0d23a6be2d506))
* **typo:**  update -> updated ([b0d2bdb0](https://github.com/linuxdeepin/dde-control-center/commit/b0d2bdb0b1c524af1a18af6f241b681561b0393d))



<a name="4.9.7"></a>
## 4.9.7 (2019-03-13)


#### Bug Fixes

*   activating indicator not show when activate a wireless from other place ([c3b3f492](https://github.com/linuxdeepin/dde-control-center/commit/c3b3f492519897a5a54e4d3883da0db85397c076))
*   sw_64 name error ([dc59f025](https://github.com/linuxdeepin/dde-control-center/commit/dc59f0257e1f83c88fcb3d4aab1638fb51154d7d))
*   frame auto hide when click choose file dialog ([d4d37b72](https://github.com/linuxdeepin/dde-control-center/commit/d4d37b72f1cc94ccc2f886cc0d6018d53683594e))
*   caption for wacom settings ([4b19bbbc](https://github.com/linuxdeepin/dde-control-center/commit/4b19bbbcce1da0b10ebe322fdb2e57eed0c68caf))
* **Update:**  wrong check update notifier ([ab8e4f73](https://github.com/linuxdeepin/dde-control-center/commit/ab8e4f7314b4aabb1e0c46a5da9ed62cf69188a2))



<a name="4.9.6"></a>
### 4.9.6 (2019-03-06)


#### Bug Fixes

*   build failed ([300b76ed](https://github.com/linuxdeepin/dde-control-center/commit/300b76ed8b4eb20944e9737561bcac38294e4ee9))



<a name="4.9.5"></a>
### 4.9.5 (2019-03-05)


#### Features

*   timeout to restore when change monitors settings ([43b5aa63](https://github.com/linuxdeepin/dde-control-center/commit/43b5aa63ebb135c7e3a3dde7e51d4245b72d1f85))

#### Bug Fixes

* **frame:**  position not align right screen box ([26c10af4](https://github.com/linuxdeepin/dde-control-center/commit/26c10af407ed05c2e4cfba3e3c68221331f73e8f))



<a name="4.9.4"></a>
### 4.9.4 (2019-02-25)


#### Bug Fixes

*   replace the hardcoded path with find_library ([b38a3f70](https://github.com/linuxdeepin/dde-control-center/commit/b38a3f7070c1a915a3f0ca35be7181a046914cd5))



<a name=""></a>
##  4.9.3 (2019-02-25)


#### Features

* **Update:**  add dependency error check ([cfb6e219](https://github.com/linuxdeepin/dde-control-center/commit/cfb6e2192374cfae9474f115616a4a6e26e0fecb))

#### Bug Fixes

*   notify date ([346b6053](https://github.com/linuxdeepin/dde-control-center/commit/346b60531fd37174ada2daf467f393bed1136354))
* **defapp:**  refresh multiple times, data is empty ([3dd033f7](https://github.com/linuxdeepin/dde-control-center/commit/3dd033f70b10a8af477a98c5745ebe83071aa7bf))



<a name="4.9.2.1"></a>
## 4.9.2.1 (2019-01-28)


#### Bug Fixes

*   network hotspot wep security ([8c0f15c6](https://github.com/linuxdeepin/dde-control-center/commit/8c0f15c64f5e39aa97c2580ecea2a2b271557f37))
* **Accounts:**  not add stretch for create page ([57e7c525](https://github.com/linuxdeepin/dde-control-center/commit/57e7c525fd6513a15ab174daaa457e8300b8ff55))

#### Features

*   sstp vpn proxy support ([8c8fb8df](https://github.com/linuxdeepin/dde-control-center/commit/8c8fb8dfc19ba4240ee68bb61a205c133bd7c385))



<a name="4.9.2"></a>
### 4.9.2 (2019-01-25)


#### Features

*   enable network proxy for sw and mipsel ([16d19139](https://github.com/linuxdeepin/dde-control-center/commit/16d191390b5956f73f04aac2c4e6a171ff493d88))
*   add support for sstp vpn ([19e9052d](https://github.com/linuxdeepin/dde-control-center/commit/19e9052d73aeba9a62657af9d82d068811cb0700))
* **Sound:**
  *  play sound ([cb7e15af](https://github.com/linuxdeepin/dde-control-center/commit/cb7e15af9ef2684c99217721140705a8f9eb6ab9))
  *  add play icon ([d539e2e0](https://github.com/linuxdeepin/dde-control-center/commit/d539e2e082bbee8951909b879aca4b6d460578dd))
  *  add sound effect page ([d57f9ad4](https://github.com/linuxdeepin/dde-control-center/commit/d57f9ad4a55e651df8aa011ae3d41a815765d837))

#### Bug Fixes

*   visible of tls section of openvpn when create edit page ([71ffe4b4](https://github.com/linuxdeepin/dde-control-center/commit/71ffe4b45de21add5f392cc8255b6c5d192120c7))
*   notify time not display completely ([12e030bf](https://github.com/linuxdeepin/dde-control-center/commit/12e030bf124177b244ef28651724ac72e80b975f))
*   build warning ([bc31b573](https://github.com/linuxdeepin/dde-control-center/commit/bc31b5737ec4cca7ec0a6d688af570cd82d5ff84))
*   macaddress ([3dfe0c26](https://github.com/linuxdeepin/dde-control-center/commit/3dfe0c26a83fca1411b1555f2fed2bebd89306e4))
* **Accounts:**
  *  wrong handle lowercase username ([0e2bb310](https://github.com/linuxdeepin/dde-control-center/commit/0e2bb31051d91aa8c133bc00e59cb76fcc269e28))
  *  not check password length ([6a855984](https://github.com/linuxdeepin/dde-control-center/commit/6a8559847b9bc1cde5859a3918d07bb61d6140da))
  *  check password without checking enabled ([4e639d92](https://github.com/linuxdeepin/dde-control-center/commit/4e639d920196db79d74d1a9579757c5bfb08974f))
* **Personalization:**  refresh wm when toggle wm failed ([32f269d3](https://github.com/linuxdeepin/dde-control-center/commit/32f269d3f1052192f919c8fe001ca8fa8a137752))
* **Sound:**  not load translation ([503ba8fa](https://github.com/linuxdeepin/dde-control-center/commit/503ba8fabd461e35d28f3e52e502d5d2a1fcfb02))
* **Update:**  handle the required path with update module ([7450a275](https://github.com/linuxdeepin/dde-control-center/commit/7450a275c5ff7d9639f126c2f9ef5a097e366737))



<a name="4.9.1"></a>
### 4.9.1 (2019-01-15)


#### Bug Fixes

*   shortcut title wordwrap ([5267e194](https://github.com/linuxdeepin/dde-control-center/commit/5267e1945d12acf02f8653a9d7972451b47b1fb1))



<a name=""></a>
##  4.9.0 (2019-01-10)


#### Bug Fixes

*   notify body display problem in english ([f9e87258](https://github.com/linuxdeepin/dde-control-center/commit/f9e872589f62d3f2a20a867f96ce3887d22b4bd3))
*   get audio info failed from dbus ([a740b428](https://github.com/linuxdeepin/dde-control-center/commit/a740b428bf3296bbda58504dff0e4a5a95f2ad23))
*   shortcut edit page detect conflict ([39db1205](https://github.com/linuxdeepin/dde-control-center/commit/39db12056211924b6937df7de74014e5a2df380c))
*   checkout the dialog return value for delete user ([3caa899a](https://github.com/linuxdeepin/dde-control-center/commit/3caa899a4648d3ace601cbff55b556706b153e61))
* **Accounts:**  check password same as username ([4b503d71](https://github.com/linuxdeepin/dde-control-center/commit/4b503d71bd787921e447a9a3ed62aee632cc3d98))
* **Update:**  smart mirror tips not wordwrap ([c2ce0947](https://github.com/linuxdeepin/dde-control-center/commit/c2ce09478822119a241cbb653abd083865047118))

#### Features

*   update copyright to 2019 ([407c2003](https://github.com/linuxdeepin/dde-control-center/commit/407c200328ad159ec39b8718fccde04984c7897d))
*   only amd64 use clang ([54744865](https://github.com/linuxdeepin/dde-control-center/commit/54744865409f634c95be8266939e730910029fc5))



<a name="4.8.7.2"></a>
## 4.8.7.2 (2019-01-04)


#### Bug Fixes

*   can not identify the active connection of wired ([a045f36e](https://github.com/linuxdeepin/dde-control-center/commit/a045f36ecbbe471ef5bb06481b1e13efa3cb5012))
*   palmdetect slider step ([04cd72ce](https://github.com/linuxdeepin/dde-control-center/commit/04cd72ceb3fad899fbb5e44c0f7bad14f374ddb6))
*   current keyboard layout and language not initialize ([b080a33c](https://github.com/linuxdeepin/dde-control-center/commit/b080a33c54663b5deb57f73ba377c19777436a74))
*   default directory for export/import vpn ([612f014e](https://github.com/linuxdeepin/dde-control-center/commit/612f014eb5e3f9f0a3567d6a6566f04883f65ce9))
* **Accounts:**  disable strong password for default ([201b3d17](https://github.com/linuxdeepin/dde-control-center/commit/201b3d17ef2ea969daff0c3e0816907ff5b106a4))
* **Display:**  cannot restore single screen ([1fae0a06](https://github.com/linuxdeepin/dde-control-center/commit/1fae0a06ad8da74b586e9b652144a2e503366f29))



<a name="4.8.7.1"></a>
## 4.8.7.1 (2018-12-28)


#### Bug Fixes

* **Keyboard:**  crash cause lambda ([fad7c02c](https://github.com/linuxdeepin/dde-control-center/commit/fad7c02c9bd04d903595a89024bd73e14475f593))

#### Features

*   set disable check strong password to default ([dd669de3](https://github.com/linuxdeepin/dde-control-center/commit/dd669de37fb146821df3578c53e7202c7c36fb24))



<a name="4.8.7"></a>
### 4.8.7 (2018-12-24)


#### Features

*   update description for auto-download of update ([35f374cf](https://github.com/linuxdeepin/dde-control-center/commit/35f374cf7c62edc399dc1ecd160960e35779b0db))
* **notify:**  notify add touch scroll ([50af4824](https://github.com/linuxdeepin/dde-control-center/commit/50af482410a0b8849b380562d0017a38d660c229))

#### Bug Fixes

*   current language not display after search ([0a8f9f0f](https://github.com/linuxdeepin/dde-control-center/commit/0a8f9f0f23bc9c781f4b08d913a4cc713e8c3c6e))
*   file choose button style ([64a4e1ef](https://github.com/linuxdeepin/dde-control-center/commit/64a4e1ef1c8fad14d5b785e470cac41b45aee64f))
* **Datetime:**  time setting has not default value ([e9d415c5](https://github.com/linuxdeepin/dde-control-center/commit/e9d415c522c56a3c3be4addbdc1d583001e1d524))
* **Keyboard:**  refresh data when enter next page ([30ef5439](https://github.com/linuxdeepin/dde-control-center/commit/30ef54394490b4eae043e7f1f45f36944dd2fac8))
* **defapp:**  not remove app when uninstall ([6b3c7c73](https://github.com/linuxdeepin/dde-control-center/commit/6b3c7c73af211b0b8e56037cd3cc2596b055c04c))
* **update:**  null pointer exception ([701cdd24](https://github.com/linuxdeepin/dde-control-center/commit/701cdd248d4aab5af3a0e9706d48459e48cfaf7d))



<a name="4.8.6"></a>
### 4.8.6 (2018-12-19)




<a name="4.8.5"></a>
### 4.8.5 (2018-12-19)


#### Bug Fixes

* **Systeminfo:**  end user license empty when zh_TW ([fb944704](https://github.com/linuxdeepin/dde-control-center/commit/fb944704261274b0ac1c0e692c190955e3bef3a0))
* **Update:**  crash when enter setting page after not stop scroll ([d6648890](https://github.com/linuxdeepin/dde-control-center/commit/d6648890e655c411aa144d51e3d82232169b4206))

#### Features

*   update ad domain config ([35ab375e](https://github.com/linuxdeepin/dde-control-center/commit/35ab375e63fc04a3fc744d55f86c6360de7b1d70))
* **accounts:**  create user use strong password ([a424d81e](https://github.com/linuxdeepin/dde-control-center/commit/a424d81e79ce7326b393d15529f3328c09e4b987))



<a name="4.8.4"></a>
### 4.8.4 (2018-12-18)


#### Bug Fixes

* **Keyboard:**  not save custom shortcut ([48e3d49e](https://github.com/linuxdeepin/dde-control-center/commit/48e3d49e77a7fc9ed94c25fee78db08ac51c1199))
* **Touch:**  use DWindowManagerHelper::currentWorkspcaeWindowIdList ([68310493](https://github.com/linuxdeepin/dde-control-center/commit/68310493b92af1ebdf4b078b09669b2371b99b66))
* **Update:**  smart mirror dbus wrong path ([317b0888](https://github.com/linuxdeepin/dde-control-center/commit/317b0888edd14d462e260895aef528d529f5b87c))

#### Features

*   add dtk version for virtualkeyboard geometry check ([3b9c8c0c](https://github.com/linuxdeepin/dde-control-center/commit/3b9c8c0c44cfecc0c5acb355628a536b806d816e))
*   edit directly a wireless after it activate failed (need more config) ([44f7e3ba](https://github.com/linuxdeepin/dde-control-center/commit/44f7e3ba4c4a13c728260b5442c3599ff515b8cd))
*   strong password ([877a8542](https://github.com/linuxdeepin/dde-control-center/commit/877a85425aa50335981c277e97ee521ff705a6ab))



<a name="4.8.3"></a>
### 4.8.3 (2018-12-17)


#### Features

* **Update:**  add smart mirror switch ([8be4978f](https://github.com/linuxdeepin/dde-control-center/commit/8be4978f4969a18d017e085f6aa09f7691dfd5e6))



<a name="4.8.2"></a>
### 4.8.2 (2018-12-13)


#### Features

*   update end user license agreement ([e481a5d9](https://github.com/linuxdeepin/dde-control-center/commit/e481a5d968c51b83a567f974d1c2e1a6670e24ee))

#### Bug Fixes

*   ipv6 address can not be displayed completely ([38b5f489](https://github.com/linuxdeepin/dde-control-center/commit/38b5f489e639e35a3f3f09275639a78f83f9ee01))
* **systeminfo:**  error version function ([bacb3465](https://github.com/linuxdeepin/dde-control-center/commit/bacb3465a38565e06341673cf251fc4d4f3feee0))



<a name="4.8.1"></a>
### 4.8.1 (2018-12-12)


#### Bug Fixes

* **Update:**
  *  hide autodownload tip when update notify switched ([dd309ff2](https://github.com/linuxdeepin/dde-control-center/commit/dd309ff22078b98c91c1cfa0afa96100055ff4c5))
  *  turn off autodownload when hide update notify ([ce6b332c](https://github.com/linuxdeepin/dde-control-center/commit/ce6b332c2c9827670aebd05e159c078c4c65dd08))
* **notify:**  update close button svg ([bf13bb76](https://github.com/linuxdeepin/dde-control-center/commit/bf13bb761b8ef1e0ea9b6f5f59f4dcf97e43b473))

#### Features

* **Keyboard:**  add touch scroll ([c3ffda71](https://github.com/linuxdeepin/dde-control-center/commit/c3ffda718d29b1b08af102ee2bb3f8ece047e5a4))
* **Notify:**  optimation list view ([c825cc73](https://github.com/linuxdeepin/dde-control-center/commit/c825cc73247e5537aabf65dbef9d1cb521b4e0e6))



<a name=""></a>
##  4.8.0 (2018-12-07)


#### Bug Fixes

*   can not save ipv6 method if set method as Ignore ([8420a862](https://github.com/linuxdeepin/dde-control-center/commit/8420a862dfb0b30ef871e90eb53135fb76f3fa36))
*   save user password as group password ([7b7f59bf](https://github.com/linuxdeepin/dde-control-center/commit/7b7f59bf6f5049f24bcfcdf69e35c5eb22cd9eb4))
* **Bluetooth:**  do not show device list when re-enter after turning off the device ([4c041bc2](https://github.com/linuxdeepin/dde-control-center/commit/4c041bc24424c7aa42f0df6e8fa7cc028e248b84))
* **Defapp:**  crash because repeated signal ([be5197c7](https://github.com/linuxdeepin/dde-control-center/commit/be5197c775a0aeb3e8e212ee29bb703983357133))
* **Display:**  ui scale value error when init ([efd22ea4](https://github.com/linuxdeepin/dde-control-center/commit/efd22ea4a23231c40086938818d83b4178d6823c))
* **Grub:**  limited for mime type ([11699b01](https://github.com/linuxdeepin/dde-control-center/commit/11699b0169d1aa704783b5afbaad8baa589a45a3))
* **Update:**
  *  changelog draw exception ([a801728d](https://github.com/linuxdeepin/dde-control-center/commit/a801728d44765837b5980e3df77a2af25c3e9c83))
  *  two dde package ([267d9a81](https://github.com/linuxdeepin/dde-control-center/commit/267d9a8128b228bfba84ce9b8fe7ce2b23d1b785))
* **network:**
  *  ip address(gateway) not be reset when methon is set to automatic ([38468490](https://github.com/linuxdeepin/dde-control-center/commit/384684900b9df5ccd68cfb65a12a91cdbb1298b8))
  *  prefix of ipv6 init invalid ([3819aafc](https://github.com/linuxdeepin/dde-control-center/commit/3819aafcde5e4596afe718406077b2fba8de6ec5))
  *  reset default auth methon in FASH,TTLS,PEAP ([98ace548](https://github.com/linuxdeepin/dde-control-center/commit/98ace548b5ad8acca5b3a8d9ab1a6dcff6d0d8ba))
* **update:**  update button is not disabled when low power ([4c92a5ab](https://github.com/linuxdeepin/dde-control-center/commit/4c92a5ab20de6d86b6fdc9d8203c7cac4a7ebb86))

#### Features

* **Update:**  add update notify switch button ([96cdfe2a](https://github.com/linuxdeepin/dde-control-center/commit/96cdfe2a16b24f9abaf798976a11a59d29c8b554))



<a name="4.7.9"></a>
### 4.7.9 (2018-12-04)


#### Bug Fixes

* **Display:**  scale set failed ([e70ba3f3](https://github.com/linuxdeepin/dde-control-center/commit/e70ba3f3abafae9499e377a11aee5bb555858a3a))
* **Power:**  disable power setting for not have battery ([3ebe3b14](https://github.com/linuxdeepin/dde-control-center/commit/3ebe3b146c59474d1029eb11107f685d3d5a1382))

#### Features

*   do not hide when clicking on the virtual keyboard ([d16d4ddf](https://github.com/linuxdeepin/dde-control-center/commit/d16d4ddf50a41321da9944220a3426da61bb693a))



<a name="4.7.8"></a>
### 4.7.8 (2018-11-30)


#### Features

* **Display:**  support uiScale to 3x ([89163013](https://github.com/linuxdeepin/dde-control-center/commit/89163013e6a6f98387c76dbb36fbd0f6146f99e0))
* **Personalization:**  add two ends icon for opacity ([2138e4ef](https://github.com/linuxdeepin/dde-control-center/commit/2138e4ef32531120d2a8a4bc232238b95790417e))
* **Power:**  separate power settings ([20e23b2d](https://github.com/linuxdeepin/dde-control-center/commit/20e23b2d511f7255d1e73672a5488379bc5de49f))

#### Bug Fixes

* **update:**
  *  Refresh list after automatic download ([a52adc90](https://github.com/linuxdeepin/dde-control-center/commit/a52adc90ba1d554ab19416b9c0eb87d780727458))
  *  repeated system patch info item ([e32f3ff6](https://github.com/linuxdeepin/dde-control-center/commit/e32f3ff60bc40a56a8f2044042d511cb3fb2c4e8))



<a name=""></a>
##  4.7.7 (2018-11-23)


#### Bug Fixes

*   replace emit to Q_EMIT ([0f26b936](https://github.com/linuxdeepin/dde-control-center/commit/0f26b93636b74a17463c9da792e7b19534b56e36))
*   pop up all pages about edit connection after device is removed ([c28e3460](https://github.com/linuxdeepin/dde-control-center/commit/c28e34607d6c6a14a186e9fccd404ff4ac70397f))
* **grub:**  disable scrollbar content menu ([f801d949](https://github.com/linuxdeepin/dde-control-center/commit/f801d949a69f9855157b535a9c6f040b18426d16))
* **keyboard:**  null pointer exeception ([787ff86b](https://github.com/linuxdeepin/dde-control-center/commit/787ff86b35914bb05a3e45d4ccd3dab1083ee970))
* **mainwidget:**  datetime label wrap. ([71d55596](https://github.com/linuxdeepin/dde-control-center/commit/71d5559642d76adf893f4cb39bc3f7ee9f65e93b))

#### Features

* **Personalization:**  remove opacity slider number ([2269b923](https://github.com/linuxdeepin/dde-control-center/commit/2269b923e8bf47320dc5c2d18eece11dc4ef9a2d))
* **grub:**  allow drop ([1eb52ab5](https://github.com/linuxdeepin/dde-control-center/commit/1eb52ab58af7f2b9f9fbc0898b6a63ce31a1e7c3))



<a name=""></a>
##  4.7.6.2 (2018-11-22)


#### Bug Fixes

*   build faild ([0dcc343c](https://github.com/linuxdeepin/dde-control-center/commit/0dcc343c1643259b0fed85b8fd45691042361ee0))



<a name=""></a>
##  4.7.6.1 (2018-11-19)


#### Bug Fixes

* **display:**  missing memory header ([24f8cf6e](https://github.com/linuxdeepin/dde-control-center/commit/24f8cf6ea299f73ee7a0e7f64fd3503bf1eb0b2c))
* **keyboard:**  null pointer exeception ([f06d35b8](https://github.com/linuxdeepin/dde-control-center/commit/f06d35b8ce0fd50a724b80455b56e5285747b5a2))



<a name="4.7.6"></a>
### 4.7.6 (2018-11-13)


#### Bug Fixes

* **display:**  save multiple screen config list failed ([951a101e](https://github.com/linuxdeepin/dde-control-center/commit/951a101e5df38eb157075a0bf124ff9d66dc11a3))
* **keyboard:**  no abbreviation for modifier keys ([9699eeb9](https://github.com/linuxdeepin/dde-control-center/commit/9699eeb9279cd41f7a461171273488505fd2a5d9))



<a name="4.7.5"></a>
### 4.7.5 (2018-11-13)


#### Bug Fixes

* **defapp:**  dbus data replacement is not complete ([1d22eb18](https://github.com/linuxdeepin/dde-control-center/commit/1d22eb182e80c85c23ad7f2f17e8706601963c99))



<a name="4.7.4"></a>
### 4.7.4 (2018-11-09)


#### Bug Fixes

*   auto connect after save connections ([64ca4a9a](https://github.com/linuxdeepin/dde-control-center/commit/64ca4a9a359cd12a80f0576af119e89e099ab3b3))
*   sysinfo description width. ([7fa69928](https://github.com/linuxdeepin/dde-control-center/commit/7fa699284166606bf12069e05ca60514f40dee97))
*   does not support blur will not display transparency group. ([aa062f34](https://github.com/linuxdeepin/dde-control-center/commit/aa062f34908a2416ccca1597ac0bbe54aa3f6b35))
*   can not create hotspot config when there are multiple wireless cards ([fbc41539](https://github.com/linuxdeepin/dde-control-center/commit/fbc41539057236b11d9bb06cd015d7603bffa17b))
*   missing page title ([b56b700a](https://github.com/linuxdeepin/dde-control-center/commit/b56b700a8d2f19488ec630a347f133ed1c97c2f0))
*   the state icon of dsl/pppoe connection ([e01bf903](https://github.com/linuxdeepin/dde-control-center/commit/e01bf90326c4919ac4cc33d4034a840435c9651e))
* **bluetooth:**  list exception ([8c262c58](https://github.com/linuxdeepin/dde-control-center/commit/8c262c58071ecaf56e05f8335f43c785c8c1eaf4))
* **display:**  rename config multiple times ([1d0945aa](https://github.com/linuxdeepin/dde-control-center/commit/1d0945aab8957c622fe165ae0aad3cd04d545784))
* **nav:**
  *  navigation flashing ([684dd402](https://github.com/linuxdeepin/dde-control-center/commit/684dd402dec3793f2a39073bb57b09bb9e3ff591))
  *  not hide nav tip when frame hide ([05dbfc89](https://github.com/linuxdeepin/dde-control-center/commit/05dbfc8908539166f8dec713992e8e9da345f778))
* **network:**  ignore auto dns if custom dns set in auto ipvx mode ([5bc245d9](https://github.com/linuxdeepin/dde-control-center/commit/5bc245d9f78906f19bfc9327cd5c525f9d874ca0))
* **notify:**  missing hover check ([73db5278](https://github.com/linuxdeepin/dde-control-center/commit/73db527831d721eb2c1646bc8b2dfbe7de840b0f))
* **personalization:**  multiple settings wm switch button ([2737256c](https://github.com/linuxdeepin/dde-control-center/commit/2737256c635bec01887fbc7919e615dee0a7dd4c))
* **theme:**  missing default icon ([f638320e](https://github.com/linuxdeepin/dde-control-center/commit/f638320ea083ea360b2fcae8dd8232b6a656d569))

#### Features

*   loading state for pppoe connection ([45efb870](https://github.com/linuxdeepin/dde-control-center/commit/45efb87050b3503888a3f582653f39d3f3317c87))
* **grub:**  disable drap photo ([47f1d328](https://github.com/linuxdeepin/dde-control-center/commit/47f1d328706e043a759ff1b5265dfa6e702d99b9))



<a name="4.7.3"></a>
### 4.7.3 (2018-11-02)


#### Bug Fixes

*   edit and connect to none secret wireless failed ([fdf42ea8](https://github.com/linuxdeepin/dde-control-center/commit/fdf42ea862573d4acf467e931063dc8f673c589d))
*   can not save pppoe connection ([30506ff1](https://github.com/linuxdeepin/dde-control-center/commit/30506ff1f110c9ff28d7dc12fea06477019753c1))
*   translation of Ethernet ([0d846acd](https://github.com/linuxdeepin/dde-control-center/commit/0d846acda98df62813635d33ca289a10018c7f2b))
* **nav:**  check bluetooth is can be used ([5c6ed732](https://github.com/linuxdeepin/dde-control-center/commit/5c6ed7322eaf208fca22f7c54138486248b65f3e))



<a name="4.7.2"></a>
### 4.7.2 (2018-11-01)


#### Features

* **grub:**  use new style for background ([22758a49](https://github.com/linuxdeepin/dde-control-center/commit/22758a4927762712606777d75a1357e991b28933))
* **keyboard:**
  *  support load file button svg ([670a86d4](https://github.com/linuxdeepin/dde-control-center/commit/670a86d4db8b639783c6f2b8318de86d420f028d))
  *  add new conflit color ([04844a6d](https://github.com/linuxdeepin/dde-control-center/commit/04844a6d495f2c9ad622af7360217898243b5cf9))
* **navbar:**  add module tip ([3a2ab7f3](https://github.com/linuxdeepin/dde-control-center/commit/3a2ab7f3b27e914791fbe2e0cb2facd59acb23f0))

#### Bug Fixes

*   float arithmetic error ([a7330801](https://github.com/linuxdeepin/dde-control-center/commit/a7330801bc51c106ab986fb4c308749001fb8c9c))
*   immediate segfault when sys update is disabled ([f9474118](https://github.com/linuxdeepin/dde-control-center/commit/f947411871beb777c82ab151718147b57939dbf9))
*   wired edit page ethernet section device address ([ef24ce50](https://github.com/linuxdeepin/dde-control-center/commit/ef24ce50fa1d4d0829a5e7a7118326cec4351ce9))
*   connection edit page bugs ([4a160050](https://github.com/linuxdeepin/dde-control-center/commit/4a160050abe1c8cf980c754e36a74f4c0e39ce2f))
* **display:**  monitor indicator not hide ([b9bb8da6](https://github.com/linuxdeepin/dde-control-center/commit/b9bb8da65bb8b2b0d671c05fde8dfa7db5bad417))
* **frame:**  coordinate precision loss ([0e0d4547](https://github.com/linuxdeepin/dde-control-center/commit/0e0d4547a833aa1219f4ad192b22efc4dae84e14))
* **gpl:**  not load zh_TW ([0a4c80ca](https://github.com/linuxdeepin/dde-control-center/commit/0a4c80ca37523293f6be5ff8f266a18447e1cd03))
* **keyboard:**  missing title ([5ccb60de](https://github.com/linuxdeepin/dde-control-center/commit/5ccb60de9bde185777427efceaec01f0c882a2a2))
* **nav:**  loss of precision ([6b5860b6](https://github.com/linuxdeepin/dde-control-center/commit/6b5860b6e2a593bd3cd6163919d7d1c5447927f5))
* **notify:**  use an extra piece of data to update the hover ([fa239be4](https://github.com/linuxdeepin/dde-control-center/commit/fa239be4b71ad6dc5eda5ce1f1c1dd7fd3d74806))



<a name="4.7.1"></a>
### 4.7.1 (2018-10-26)


#### Bug Fixes

* **keyboard:**  null pointer execption ([09678cee](https://github.com/linuxdeepin/dde-control-center/commit/09678ceeb304534dd66facf1d856d4dbbdf09eb9))



<a name=""></a>
##  4.7.0 (2018-10-25)


#### Features

*   move notify toggle button 20px away from the right side ([331f7813](https://github.com/linuxdeepin/dde-control-center/commit/331f78135bbf2bb76d0b2152d7e4db11760d1caf))
*   update opacity list ([a5946c60](https://github.com/linuxdeepin/dde-control-center/commit/a5946c604b03966f58fb3c1b7084a9bb13ec189c))
*   change full screenshot position ([8dfbe50a](https://github.com/linuxdeepin/dde-control-center/commit/8dfbe50a9e4ec6c619455a14dab54d71c2e9f3bb))
*   hide empty item for navigation ([9ecc8978](https://github.com/linuxdeepin/dde-control-center/commit/9ecc8978dd876a9600d4c8ded292db8ffcc56301))
*   navigation add rectangular round angle ([39c56f6f](https://github.com/linuxdeepin/dde-control-center/commit/39c56f6fe08b7ada77ac2cd0b14901b4035a45c3))
*   add notify toggle button ([91ced2de](https://github.com/linuxdeepin/dde-control-center/commit/91ced2def2f791a1e6b43e880eefdb332b028ff8))
*   support change frame opacity ([650a151f](https://github.com/linuxdeepin/dde-control-center/commit/650a151fe980ca046fad42c0d970986606ce8acb))
*   use new gpl chinese license ([a6a0d761](https://github.com/linuxdeepin/dde-control-center/commit/a6a0d761ca30479751227e08e62759854a798ff1))
*   update license ([05637776](https://github.com/linuxdeepin/dde-control-center/commit/056377760a9ee7995ff42e5da650fba737d06376))
*   update license ([2c53e92b](https://github.com/linuxdeepin/dde-control-center/commit/2c53e92bcb903084d709d2b2ef4657e92ca529f2))
*   add end-user-license ([3a307825](https://github.com/linuxdeepin/dde-control-center/commit/3a3078252b483c23d624fd05a1002e007ec128b5))
* **brightness:**  ambient light sensor support ([d9011601](https://github.com/linuxdeepin/dde-control-center/commit/d9011601d6cced649e35fed813f89ae5c92306d8))
* **display:**
  *  support save tmp display config ([73d7d938](https://github.com/linuxdeepin/dde-control-center/commit/73d7d938ae6884c95ea5ede150c66ad572425bf0))
  *  add display control listview ([8a72fc8f](https://github.com/linuxdeepin/dde-control-center/commit/8a72fc8f88cb36bedc8dc93c8112ec24377ca268))
* **personalization:**
  *  change opacity range ([458385f0](https://github.com/linuxdeepin/dde-control-center/commit/458385f0ac166ae687873672f8cb9dbf23d18884))
  *  support set frame transparent ([a94b8f08](https://github.com/linuxdeepin/dde-control-center/commit/a94b8f0861a3930ec4c245c2b74ae940a5ed39c6))
* **power:**  disable powersave for sw and mips64 ([c387324f](https://github.com/linuxdeepin/dde-control-center/commit/c387324f99d5ffad4d3691ac5bd009d7912cd3db))
* **pro:**  set auto start for mips and sw ([1d0ce544](https://github.com/linuxdeepin/dde-control-center/commit/1d0ce5442b3f4d9b792e7bebba469cd0181af0d4))
* **shortcut:**  add full screenshort ([9a43cd1e](https://github.com/linuxdeepin/dde-control-center/commit/9a43cd1eebf243fa8110c60eeb82b014cde6c6fe))

#### Bug Fixes

*   some connection edit bugs ([9c978f24](https://github.com/linuxdeepin/dde-control-center/commit/9c978f242423d464625e5101a18f49bdaff4814c))
*   translation of hotspot edit page ([4043e35d](https://github.com/linuxdeepin/dde-control-center/commit/4043e35d0a21ee7de20dec3c50c29958d14c13e8))
*   add default button for reminder dialog ([824c29b4](https://github.com/linuxdeepin/dde-control-center/commit/824c29b4f11b7ba8a24e19a365ff970a567c341f))
*   timezone map and monitor settings dialog show on frame ([b2f97182](https://github.com/linuxdeepin/dde-control-center/commit/b2f97182d9088bcbbd948ee53ffaa2190d6d3db3))
*   namespace ([c8bef6ff](https://github.com/linuxdeepin/dde-control-center/commit/c8bef6ff946143b05aa03200d13d89d7a68d2e5f))
*   active connection identify ([5adaa246](https://github.com/linuxdeepin/dde-control-center/commit/5adaa24611d0b4401f9b34a946e414cea15aeadb))
*   navigation text pos error ([ce35abee](https://github.com/linuxdeepin/dde-control-center/commit/ce35abee44e0a6ae48758e7daa9d4535162ea4d3))
*   translation string typo error ([83f99581](https://github.com/linuxdeepin/dde-control-center/commit/83f99581ee95f75daa3574fffeef35e6a9ab7981))
*   navigation not use scale ratio ([838b3c8e](https://github.com/linuxdeepin/dde-control-center/commit/838b3c8e192fa5295261f0ec623b1a62b12daef4))
*   translations after network refactor ([0ffc3c70](https://github.com/linuxdeepin/dde-control-center/commit/0ffc3c702b986541a4573e421f1d9df1eed5e1d6))
*   mission bin ([6e43c431](https://github.com/linuxdeepin/dde-control-center/commit/6e43c431de522095d622a8118ab0e2cb368fce2b))
*   not enable autostart for mips and sw64 ([e7bbcffb](https://github.com/linuxdeepin/dde-control-center/commit/e7bbcffb8f3a2233492a3e3dc5d97a8d9abd0938))
*   quick control wifi bugs ([6ab587a8](https://github.com/linuxdeepin/dde-control-center/commit/6ab587a89742a61f976bce155648e5a9a1e5f0e8))
*   lost macro controlled by bluetooth ([0354de17](https://github.com/linuxdeepin/dde-control-center/commit/0354de17d5b84dc7f4a452750d7510a9c54ad7f3))
*   remove excess spacing ([92be5f5c](https://github.com/linuxdeepin/dde-control-center/commit/92be5f5cb4b5c3714595c52e4a2588f6da151e1a))
* **bluetooth:**  check dbus service is valid ([40d33330](https://github.com/linuxdeepin/dde-control-center/commit/40d33330bbbe1fcee553254fad67ba5ffbd17c3b))
* **datetime:**  frame cannot auto hide ([09931604](https://github.com/linuxdeepin/dde-control-center/commit/09931604ea1856432421c5c2ac6474d4d652c43d))
* **frame:**  incorrect coordinate when primary screen changed ([2a3cf613](https://github.com/linuxdeepin/dde-control-center/commit/2a3cf613ec2985dce81875d9b7d97242a5f872ca))
* **keyboard:**  update conflict shortcut item ([6405d70d](https://github.com/linuxdeepin/dde-control-center/commit/6405d70d0ab83c3210eb0f4a928b3178d413ea12))
* **nav:**  text offset ([4489d5b2](https://github.com/linuxdeepin/dde-control-center/commit/4489d5b22a7ad4ddf4d3b2239b27cd43295af1a9))
* **plugins:**  temperature not fully use fahrenheit ([8ce53f37](https://github.com/linuxdeepin/dde-control-center/commit/8ce53f378acc4c996a9c5fe3686e4747a01e42ee))
* **quick:**  error of high judgement ([8cc10d05](https://github.com/linuxdeepin/dde-control-center/commit/8cc10d050fe4d20b26c06761da18523ae41c211f))
* **shortcut:**  replace shortcut without waiting to complete ([fb227595](https://github.com/linuxdeepin/dde-control-center/commit/fb227595b81b20994fcaae908f41d20061ce85ff))
* **sw:**  mission mieee ([b93ac2cd](https://github.com/linuxdeepin/dde-control-center/commit/b93ac2cdb17ba9e2d8e23e4d0d0b68ae23b99faf))
* **systeminfo:**  gpl file name error ([57339a2f](https://github.com/linuxdeepin/dde-control-center/commit/57339a2f0b6c57f6ed2a9e6d05921f004bf467bc))
* **update:**  clean other job when failed ([819cd824](https://github.com/linuxdeepin/dde-control-center/commit/819cd824760b04d5fb98d04b9186d8b68ff42d69))



<a name=""></a>
##  4.6.4.11 (2018-10-26)


#### Bug Fixes

* **keyboard:**  null pointer exeception ([a9a41ee9](https://github.com/linuxdeepin/dde-control-center/commit/a9a41ee9500c29d4f95fc60fb3304f58360f1120))



<a name=""></a>
##  4.6.4.10 (2018-10-24)


#### Bug Fixes

* **quick:**  missing disable bluetooth macro ([25335239](https://github.com/linuxdeepin/dde-control-center/commit/25335239f2afb3433defbfc9706b5a9762557c84))



<a name=""></a>
##  4.6.4.9 (2018-10-12)


#### Features

* **power:**  disable powersave for sw and mips64 ([8c45fd74](https://github.com/linuxdeepin/dde-control-center/commit/8c45fd74e81f74165672e5f3a6892a5640d6760e))



<a name=""></a>
##  4.6.4.8 (2018-09-27)


#### Features

*   merge operations in a short time for quick control panel ([f354796d](https://github.com/linuxdeepin/dde-control-center/commit/f354796d61cd7ae60c9a6404ead6a3494409faa9))



<a name=""></a>
##  4.6.4.7 (2018-09-19)


#### Features

*   update license ([2d3bbea2](https://github.com/linuxdeepin/dde-control-center/commit/2d3bbea2d4ac7d3761770965e16e635a3e50fef4))

#### Bug Fixes

* **datetime:**  frame cannot auto hide ([eae38fc4](https://github.com/linuxdeepin/dde-control-center/commit/eae38fc493f249b8d4619b4698c37afcfc2297d0))
* **shortcut:**  replace shortcut without waiting to complete ([ce6e279c](https://github.com/linuxdeepin/dde-control-center/commit/ce6e279c9d6fdb503136f5a55133119fb597e708))



<a name=""></a>
##  4.6.4.6 (2018-09-13)


#### Bug Fixes

*   mission bin ([0afc93ae](https://github.com/linuxdeepin/dde-control-center/commit/0afc93ae08efacd5aa34167ca3f0e6c8260e9346))



<a name=""></a>
##  4.6.4.5 (2018-09-13)


#### Bug Fixes

*   not enable autostart for mips and sw64 ([78ce7631](https://github.com/linuxdeepin/dde-control-center/commit/78ce7631bfee919434b8f12bc102293ee8dd3e95))



<a name=""></a>
##  4.6.4.4 (2018-09-07)




<a name=""></a>
##  4.6.4.3 (2018-09-07)




<a name=""></a>
##  4.6.4.2 (2018-09-07)


#### Features

* **pro:**  set auto start for mips and sw ([4eef110f](https://github.com/linuxdeepin/dde-control-center/commit/4eef110fd9536be89f56fab3450f39bbd909857f))



<a name=""></a>
##  4.6.4.1 (2018-08-31)


#### Bug Fixes

* **sw:**  mission mieee ([d804ccf3](https://github.com/linuxdeepin/dde-control-center/commit/d804ccf3ee583b0e03672c642b8a77835bf3d994))



<a name="4.6.4"></a>
### 4.6.4 (2018-08-12)


#### Bug Fixes

*   back button of mouse clicked on frame ([729cdcc0](https://github.com/linuxdeepin/dde-control-center/commit/729cdcc06e45a308c5f8930b0e2749af7f2f9d33))
* **network:**  hotspot page refresh ([117e5069](https://github.com/linuxdeepin/dde-control-center/commit/117e5069afa453d14dae20073871bfcda2109fac))



<a name="4.6.3"></a>
### 4.6.3 (2018-08-07)


#### Bug Fixes

* **display:**  brightness error when monitor changed ([3a881312](https://github.com/linuxdeepin/dde-control-center/commit/3a881312575eb09e9a2eac6af26d3421403905db))
* **notify:**  high screen filament ([83d17c9d](https://github.com/linuxdeepin/dde-control-center/commit/83d17c9d38fe828bd8e8a239a4fb775b541534ef))



<a name="4.6.2"></a>
### 4.6.2 (2018-07-31)


#### Bug Fixes

* **accounts:**  disable modify the fingerprints of other users ([04dcfed6](https://github.com/linuxdeepin/dde-control-center/commit/04dcfed6d63ccdd72dbcb242265b470743aef3d9))
* **bluetooth:**  Iterator failure ([2e820733](https://github.com/linuxdeepin/dde-control-center/commit/2e82073374d5b6d094057537881bd6c99f985820))
* **network:**
  *  scan wireless ([f18bf436](https://github.com/linuxdeepin/dde-control-center/commit/f18bf436ff6d10aa4710283ab70ec6e15f29ebda))
  *  uuid invalid of ap-connection ([efb4786c](https://github.com/linuxdeepin/dde-control-center/commit/efb4786c64f8b2eceb618f1de072c7f9b4ef9292))

#### Features

* **mouse:**  Ability to set accel profile ([d2c88c59](https://github.com/linuxdeepin/dde-control-center/commit/d2c88c5939a89a5f5f4b5cb7d48d6652d9e308bd))
* **network:**  set filter for import vpn ([196c1fc2](https://github.com/linuxdeepin/dde-control-center/commit/196c1fc2b31d0db80f01c8d247ac695b85e1cb27))
* **weather:**  Hide the tips after one minute of the refresh of the weather ([c4aca867](https://github.com/linuxdeepin/dde-control-center/commit/c4aca8673a2c1ca275a3c5881223a584a014eb0f))

<a name="4.6.1"></a>
### 4.6.1 (2018-07-20)




<a name=""></a>
##  4.6.0 (2018-07-20)


#### Bug Fixes

*   have prohibit drag and drop icons ([a262c6f5](https://github.com/linuxdeepin/dde-control-center/commit/a262c6f56e60e41947ecd04b0818eb22732ec2d1))
*   the window shadow joggle when window show animation finished ([444cb257](https://github.com/linuxdeepin/dde-control-center/commit/444cb257dc83a71693c17e8d989bef3a73135e83))
*   wifi quick control obtain AP info ([f2f6f2a9](https://github.com/linuxdeepin/dde-control-center/commit/f2f6f2a9099184d62aa5f52a1ca6c8c4c69bfff2))
*   Activated connection can't disconnect ([ac0b3c8c](https://github.com/linuxdeepin/dde-control-center/commit/ac0b3c8c2be4a07340108da8a0f62c1ce15250c1))
*   use new functions in new Qt ([170602e2](https://github.com/linuxdeepin/dde-control-center/commit/170602e250db333dd55c588a0c3227fb7766d870))
*   navbar item selected is incorrect. ([b4c98260](https://github.com/linuxdeepin/dde-control-center/commit/b4c98260cad75061411ad61f6092f6402aa9a954))
*   update the status of the mpris picture. ([c4d2e64c](https://github.com/linuxdeepin/dde-control-center/commit/c4d2e64cc31ca1a2f2ea9412cf312ff2d84a4be1))
*   navigation pixmap incorrect rect in high dpi ([c2f52680](https://github.com/linuxdeepin/dde-control-center/commit/c2f526809e4d4d043609e6c3cbc90951616caf3a))
*   vertical ScrollMode ([93d0645c](https://github.com/linuxdeepin/dde-control-center/commit/93d0645c51d26b24c57da4466bb3e1fc179d883f))
*   translation ([f846337a](https://github.com/linuxdeepin/dde-control-center/commit/f846337ae9d3315ca9a90bc61a16a239f011a676))
*   time of notify visible ([44ac6e1a](https://github.com/linuxdeepin/dde-control-center/commit/44ac6e1ad6bfa503403615204c2e93c2d4d2f449))
*   click on blank item will open settings ([b4cbf273](https://github.com/linuxdeepin/dde-control-center/commit/b4cbf273cfe9c07adb360b9cf742131cd089a7ca))
*   left mouse button gesture does not refresh navbar. ([3535b9a9](https://github.com/linuxdeepin/dde-control-center/commit/3535b9a96be42bafeba38c39b21d3a5d16bf5ca0))
*   crash if no connections info ([8bdab525](https://github.com/linuxdeepin/dde-control-center/commit/8bdab52599884bd38149ee8ab6a8aed0ea8dd4b2))
*   gsettings key error ([20c16a89](https://github.com/linuxdeepin/dde-control-center/commit/20c16a89d321495575d688baedb7f460c2035b0c))
*   prevent the new password is equal the old ([840959b0](https://github.com/linuxdeepin/dde-control-center/commit/840959b074f47551aa80b06b8760968e7e8293df))
*   the maximum volume is 150 ([4ce223b4](https://github.com/linuxdeepin/dde-control-center/commit/4ce223b48c5f7b31ae7dcdfab527d7884f8aa952))
*   incorrect time zone ([aa866c7d](https://github.com/linuxdeepin/dde-control-center/commit/aa866c7dbe86880b85f444726ab0ab9af073f93f))
* **Network:**  not update vpn type ([459bb57e](https://github.com/linuxdeepin/dde-control-center/commit/459bb57e5d7d8b48b344b16d472d9cdfe31cd93a))
* **Personalization:**  refresh wm state when toggle wm ([da4323d7](https://github.com/linuxdeepin/dde-control-center/commit/da4323d7590367eae061be5290c34e8c3d8b20a4))
* **accounts:**
  *  memory leak ([5583d2a2](https://github.com/linuxdeepin/dde-control-center/commit/5583d2a2d8f54dc5845bc43120acb74ab8d71bed))
  *  hide alert when lineedit editing finished ([2518b7cd](https://github.com/linuxdeepin/dde-control-center/commit/2518b7cd5623f3f48bea23a69af34c84693094b9))
  *  hide all alert when edit changed ([c85bf972](https://github.com/linuxdeepin/dde-control-center/commit/c85bf972a618066db9dbbbeade7b9b1c57b04374))
  *  error tip not hide when frame hide ([3234fb0b](https://github.com/linuxdeepin/dde-control-center/commit/3234fb0b517235ee0ad6f76c0e7ddecf3b54a911))
  *  modify avatar page not use fullname set title ([f5254130](https://github.com/linuxdeepin/dde-control-center/commit/f525413056586080ec9cf47e6795df79e4c60870))
  *  not refresh user online state ([a03ecf81](https://github.com/linuxdeepin/dde-control-center/commit/a03ecf818729863f83e9039bbc8940c097c31282))
* **date:**  remove lineedit fixwidth ([4a6448fd](https://github.com/linuxdeepin/dde-control-center/commit/4a6448fd24fe8675185e0b5942eaf1ef2d7c2275))
* **datetime:**
  *  not hide frame casue dialog confirmed ([d80f1804](https://github.com/linuxdeepin/dde-control-center/commit/d80f18049a5de2136a19474e3c2ce6ca510ddd48))
  *  delete signals that do not need to be send ([3d75c6aa](https://github.com/linuxdeepin/dde-control-center/commit/3d75c6aa3f575221a7467549e3837eff4b604399))
  *  system time zone not initialize ([b7e25c08](https://github.com/linuxdeepin/dde-control-center/commit/b7e25c08b2a6554c799ae810057cbd1a295c8d6b))
* **display:**
  *  memory leak ([e412745a](https://github.com/linuxdeepin/dde-control-center/commit/e412745aeb44dea73440d27276b104c10f8d9814))
  *  set frame auto hide when multiple screen setting dialog show ([f0a333f8](https://github.com/linuxdeepin/dde-control-center/commit/f0a333f880ad0cfab8c9b9c795edf7eb910bca51))
  *  switch button will be disabled when changing night mode ([b0dd4d9d](https://github.com/linuxdeepin/dde-control-center/commit/b0dd4d9d7de4dc112764cb8bdcc6909e2be4e492))
* **frame:**
  *  datetime label not word wrap ([1252dd6a](https://github.com/linuxdeepin/dde-control-center/commit/1252dd6ad110cf1c72a96cbb1663c9150fdd740a))
  *  error sending geometry at the end of the animation ([01b30562](https://github.com/linuxdeepin/dde-control-center/commit/01b30562d7738fbb68d7d2b1fefbbef350130952))
* **keyboard:**
  *  error set DisableIfTyping value ([270c52ae](https://github.com/linuxdeepin/dde-control-center/commit/270c52aef938cc714eea93c78f9450552ce2fd6e))
  *  title breaking ([b7afa0de](https://github.com/linuxdeepin/dde-control-center/commit/b7afa0de5163ebc619d68b400f9103db72f8e487))
* **live:**  Failure of live system compilation ([8149c9a6](https://github.com/linuxdeepin/dde-control-center/commit/8149c9a6956b04ca01af799b9e58ec43c407d872))
* **network:**
  *  item not mark as selected ([b142de39](https://github.com/linuxdeepin/dde-control-center/commit/b142de3992d053e83d0495e2d56c1331e72fcee0))
  *  set app proxy password hiding ([6c660d39](https://github.com/linuxdeepin/dde-control-center/commit/6c660d394d9dedfbf36aa2a51435adc3128d3faa))
  *  update complie macro ([bf555941](https://github.com/linuxdeepin/dde-control-center/commit/bf55594157f2bab42abe3b619417bf0bf29c4e67))
* **notify:**  icon missing ([e72aeca1](https://github.com/linuxdeepin/dde-control-center/commit/e72aeca19fb3720e2b728ceb61b903ba03e3b19d))
* **quick-wifi:**  multiple same ssid ap ([67d0947a](https://github.com/linuxdeepin/dde-control-center/commit/67d0947acd1cef99284adf4d3d2a7d4f64ce227c))
* **update:**
  *  mirror test button is non-existent ([62ff76e2](https://github.com/linuxdeepin/dde-control-center/commit/62ff76e2f627b06f394736076883ef0c959741d9))
  *  changelog can select ([2f4be91f](https://github.com/linuxdeepin/dde-control-center/commit/2f4be91fb9e326559ae8b86f4af11848aa547eb0))
  *  If job is already unavailable, it needs to be deleted ([b09ac352](https://github.com/linuxdeepin/dde-control-center/commit/b09ac352be1f0f0267d0bcf682550f659bb9d144))
  *  missing percent sign ([f9dfc470](https://github.com/linuxdeepin/dde-control-center/commit/f9dfc470b05b4c64bfd34663c9d6f11a46e16539))
  *  Incorrect setting of hints ([ecb6001f](https://github.com/linuxdeepin/dde-control-center/commit/ecb6001fcf985edebefe17c9dc77c4556a927b89))
* **weather:**
  *  exceptions caused by using different data ([a366850c](https://github.com/linuxdeepin/dde-control-center/commit/a366850cb2f8bfdfe2c5b801852cb7c7ade96c08))
  *  draw background color ([86ecce5e](https://github.com/linuxdeepin/dde-control-center/commit/86ecce5e2fcf91861e483850602d9b5231c7e470))
  *  hide item when refresh city ([27ca031f](https://github.com/linuxdeepin/dde-control-center/commit/27ca031fa72f82da5541298be1c554799a69eff6))
* **zone:**  repair time offset error ([ab908a17](https://github.com/linuxdeepin/dde-control-center/commit/ab908a1727adde3a889085aeb7d192165332c7b4))

#### Performance

* **bluetooth:**  stop handle with frame hide ([b9112712](https://github.com/linuxdeepin/dde-control-center/commit/b91127124c1759d71be35bfd6c45a574c022751c))

#### Features

*   optimization refresh time for weather plugin ([c0ddf95b](https://github.com/linuxdeepin/dde-control-center/commit/c0ddf95b7f965c440eaba41974b8673868e5849d))
*   disable second page create ([5821e270](https://github.com/linuxdeepin/dde-control-center/commit/5821e270e7f1211e346859f7a3e5934abf98ae64))
*   real time sync navigation bar ([a64cbc17](https://github.com/linuxdeepin/dde-control-center/commit/a64cbc17135351acf4cf330cfa34adb1cc356cdf))
*   optimize animation ([964d324f](https://github.com/linuxdeepin/dde-control-center/commit/964d324ffe95689f108f76f2814125dc953619cf))
*   check disk space for update ([29acc794](https://github.com/linuxdeepin/dde-control-center/commit/29acc7941c0b38b84d2e0d5704e2f29eb0a1b224))
*   move auto-exit settings to gsettings ([c11c1da4](https://github.com/linuxdeepin/dde-control-center/commit/c11c1da4c9c80317fcdbd918f96210dcced4df2f))
*   encrypt password in dbus transfer ([5cac53d6](https://github.com/linuxdeepin/dde-control-center/commit/5cac53d615cab91603f5106fcba4002bba64e0c5))
* **bluetooth:**
  *  disable discover when leave bluetooth page ([6c953772](https://github.com/linuxdeepin/dde-control-center/commit/6c953772eea568a3913a81e9cc6bef0d2b975772))
  *  support close pincode dialog ([47f019bf](https://github.com/linuxdeepin/dde-control-center/commit/47f019bfc79128dce9ca5148e690d194083eea0d))
* **frame:**  use QAtomInt to set frame visible ([c6583e38](https://github.com/linuxdeepin/dde-control-center/commit/c6583e38a37e5e93ca3523356976786bffa203b2))
* **network:**
  *  load dde-network-utils translator ([bbac6bbb](https://github.com/linuxdeepin/dde-control-center/commit/bbac6bbb30fd23a32b66c9cd314eb4df40dce23b))
  *  support remove app proxy config ([906eeb27](https://github.com/linuxdeepin/dde-control-center/commit/906eeb27574b5088a3958f98d04d233eb4320bd3))
  *  hide export button in vpn create page ([2361a9aa](https://github.com/linuxdeepin/dde-control-center/commit/2361a9aa605cfad5dec31981e6e61ed3e3765caa))
* **personalization:**  country based list sort ([650edb2e](https://github.com/linuxdeepin/dde-control-center/commit/650edb2e940b17e80b4c629c5cd7ee6d76ce6117))
* **power:**  add power save mode settings ([3a894022](https://github.com/linuxdeepin/dde-control-center/commit/3a89402289b3ab1eac1d85cf5387e87bf7e2166b))
* **weather:**
  *  print all weather info ([62fde1bd](https://github.com/linuxdeepin/dde-control-center/commit/62fde1bd6b2aa041d41e283696f2e080e0589d11))
  *  add log ([e00d53ef](https://github.com/linuxdeepin/dde-control-center/commit/e00d53efdafa9cd8c8294e9bafd0d485f6f142ce))



<a name="4.5.5"></a>
### 4.5.5 (2018-06-12)


#### Bug Fixes

*   Activated connection can't disconnect ([ac0b3c8c](https://github.com/linuxdeepin/dde-control-center/commit/ac0b3c8c2be4a07340108da8a0f62c1ce15250c1))
* **weather:**  exceptions caused by using different data ([a366850c](https://github.com/linuxdeepin/dde-control-center/commit/a366850cb2f8bfdfe2c5b801852cb7c7ade96c08))



<a name="4.5.4"></a>
### 4.5.4 (2018-06-07)


#### Features

* **network:**  support remove app proxy config ([906eeb27](https://github.com/linuxdeepin/dde-control-center/commit/906eeb27574b5088a3958f98d04d233eb4320bd3))

#### Bug Fixes

* **accounts:**  hide alert when lineedit editing finished ([2518b7cd](https://github.com/linuxdeepin/dde-control-center/commit/2518b7cd5623f3f48bea23a69af34c84693094b9))
* **display:**  memory leak ([e412745a](https://github.com/linuxdeepin/dde-control-center/commit/e412745aeb44dea73440d27276b104c10f8d9814))



<a name="4.5.3"></a>
### 4.5.3 (2018-05-31)


#### Bug Fixes

*   use new functions in new Qt ([170602e2](https://github.com/linuxdeepin/dde-control-center/commit/170602e250db333dd55c588a0c3227fb7766d870))
* **datetime:**  not hide frame casue dialog confirmed ([d80f1804](https://github.com/linuxdeepin/dde-control-center/commit/d80f18049a5de2136a19474e3c2ce6ca510ddd48))
* **keyboard:**  error set DisableIfTyping value ([270c52ae](https://github.com/linuxdeepin/dde-control-center/commit/270c52aef938cc714eea93c78f9450552ce2fd6e))



<a name="4.5.2"></a>
### 4.5.2 (2018-05-30)


#### Bug Fixes

* **datetime:**  delete signals that do not need to be send ([3d75c6aa](https://github.com/linuxdeepin/dde-control-center/commit/3d75c6aa3f575221a7467549e3837eff4b604399))



<a name="4.5.1"></a>
### 4.5.1 (2018-05-29)


#### Bug Fixes

* **Network:**  not update vpn type ([459bb57e](https://github.com/linuxdeepin/dde-control-center/commit/459bb57e5d7d8b48b344b16d472d9cdfe31cd93a))
* **datetime:**  system time zone not initialize ([b7e25c08](https://github.com/linuxdeepin/dde-control-center/commit/b7e25c08b2a6554c799ae810057cbd1a295c8d6b))
* **display:**  set frame auto hide when multiple screen setting dialog show ([f0a333f8](https://github.com/linuxdeepin/dde-control-center/commit/f0a333f880ad0cfab8c9b9c795edf7eb910bca51))
* **frame:**  datetime label not word wrap ([1252dd6a](https://github.com/linuxdeepin/dde-control-center/commit/1252dd6ad110cf1c72a96cbb1663c9150fdd740a))
* **live:**  Failure of live system compilation ([8149c9a6](https://github.com/linuxdeepin/dde-control-center/commit/8149c9a6956b04ca01af799b9e58ec43c407d872))
* **weather:**  draw background color ([86ecce5e](https://github.com/linuxdeepin/dde-control-center/commit/86ecce5e2fcf91861e483850602d9b5231c7e470))

#### Features

* **frame:**  use QAtomInt to set frame visible ([c6583e38](https://github.com/linuxdeepin/dde-control-center/commit/c6583e38a37e5e93ca3523356976786bffa203b2))
* **weather:**  print all weather info ([62fde1bd](https://github.com/linuxdeepin/dde-control-center/commit/62fde1bd6b2aa041d41e283696f2e080e0589d11))



<a name=""></a>
##  4.5.0 (2018-05-24)


#### Bug Fixes

*   navbar item selected is incorrect. ([b4c98260](https://github.com/linuxdeepin/dde-control-center/commit/b4c98260cad75061411ad61f6092f6402aa9a954))
*   update the status of the mpris picture. ([c4d2e64c](https://github.com/linuxdeepin/dde-control-center/commit/c4d2e64cc31ca1a2f2ea9412cf312ff2d84a4be1))
*   navigation pixmap incorrect rect in high dpi ([c2f52680](https://github.com/linuxdeepin/dde-control-center/commit/c2f526809e4d4d043609e6c3cbc90951616caf3a))
*   vertical ScrollMode ([93d0645c](https://github.com/linuxdeepin/dde-control-center/commit/93d0645c51d26b24c57da4466bb3e1fc179d883f))
*   translation ([f846337a](https://github.com/linuxdeepin/dde-control-center/commit/f846337ae9d3315ca9a90bc61a16a239f011a676))
*   time of notify visible ([44ac6e1a](https://github.com/linuxdeepin/dde-control-center/commit/44ac6e1ad6bfa503403615204c2e93c2d4d2f449))
*   click on blank item will open settings ([b4cbf273](https://github.com/linuxdeepin/dde-control-center/commit/b4cbf273cfe9c07adb360b9cf742131cd089a7ca))
*   left mouse button gesture does not refresh navbar. ([3535b9a9](https://github.com/linuxdeepin/dde-control-center/commit/3535b9a96be42bafeba38c39b21d3a5d16bf5ca0))
*   crash if no connections info ([8bdab525](https://github.com/linuxdeepin/dde-control-center/commit/8bdab52599884bd38149ee8ab6a8aed0ea8dd4b2))
*   gsettings key error ([20c16a89](https://github.com/linuxdeepin/dde-control-center/commit/20c16a89d321495575d688baedb7f460c2035b0c))
*   prevent the new password is equal the old ([840959b0](https://github.com/linuxdeepin/dde-control-center/commit/840959b074f47551aa80b06b8760968e7e8293df))
*   the maximum volume is 150 ([4ce223b4](https://github.com/linuxdeepin/dde-control-center/commit/4ce223b48c5f7b31ae7dcdfab527d7884f8aa952))
*   incorrect time zone ([aa866c7d](https://github.com/linuxdeepin/dde-control-center/commit/aa866c7dbe86880b85f444726ab0ab9af073f93f))
* **Personalization:**  refresh wm state when toggle wm ([da4323d7](https://github.com/linuxdeepin/dde-control-center/commit/da4323d7590367eae061be5290c34e8c3d8b20a4))
* **accounts:**
  *  hide all alert when edit changed ([c85bf972](https://github.com/linuxdeepin/dde-control-center/commit/c85bf972a618066db9dbbbeade7b9b1c57b04374))
  *  error tip not hide when frame hide ([3234fb0b](https://github.com/linuxdeepin/dde-control-center/commit/3234fb0b517235ee0ad6f76c0e7ddecf3b54a911))
  *  modify avatar page not use fullname set title ([f5254130](https://github.com/linuxdeepin/dde-control-center/commit/f525413056586080ec9cf47e6795df79e4c60870))
  *  not refresh user online state ([a03ecf81](https://github.com/linuxdeepin/dde-control-center/commit/a03ecf818729863f83e9039bbc8940c097c31282))
* **date:**  remove lineedit fixwidth ([4a6448fd](https://github.com/linuxdeepin/dde-control-center/commit/4a6448fd24fe8675185e0b5942eaf1ef2d7c2275))
* **display:**  switch button will be disabled when changing night mode ([b0dd4d9d](https://github.com/linuxdeepin/dde-control-center/commit/b0dd4d9d7de4dc112764cb8bdcc6909e2be4e492))
* **frame:**  error sending geometry at the end of the animation ([01b30562](https://github.com/linuxdeepin/dde-control-center/commit/01b30562d7738fbb68d7d2b1fefbbef350130952))
* **keyboard:**  title breaking ([b7afa0de](https://github.com/linuxdeepin/dde-control-center/commit/b7afa0de5163ebc619d68b400f9103db72f8e487))
* **network:**
  *  item not mark as selected ([b142de39](https://github.com/linuxdeepin/dde-control-center/commit/b142de3992d053e83d0495e2d56c1331e72fcee0))
  *  set app proxy password hiding ([6c660d39](https://github.com/linuxdeepin/dde-control-center/commit/6c660d394d9dedfbf36aa2a51435adc3128d3faa))
  *  update complie macro ([bf555941](https://github.com/linuxdeepin/dde-control-center/commit/bf55594157f2bab42abe3b619417bf0bf29c4e67))
* **notify:**  icon missing ([e72aeca1](https://github.com/linuxdeepin/dde-control-center/commit/e72aeca19fb3720e2b728ceb61b903ba03e3b19d))
* **update:**
  *  mirror test button is non-existent ([62ff76e2](https://github.com/linuxdeepin/dde-control-center/commit/62ff76e2f627b06f394736076883ef0c959741d9))
  *  changelog can select ([2f4be91f](https://github.com/linuxdeepin/dde-control-center/commit/2f4be91fb9e326559ae8b86f4af11848aa547eb0))
  *  If job is already unavailable, it needs to be deleted ([b09ac352](https://github.com/linuxdeepin/dde-control-center/commit/b09ac352be1f0f0267d0bcf682550f659bb9d144))
  *  missing percent sign ([f9dfc470](https://github.com/linuxdeepin/dde-control-center/commit/f9dfc470b05b4c64bfd34663c9d6f11a46e16539))
  *  Incorrect setting of hints ([ecb6001f](https://github.com/linuxdeepin/dde-control-center/commit/ecb6001fcf985edebefe17c9dc77c4556a927b89))
* **weather:**  hide item when refresh city ([27ca031f](https://github.com/linuxdeepin/dde-control-center/commit/27ca031fa72f82da5541298be1c554799a69eff6))
* **zone:**  repair time offset error ([ab908a17](https://github.com/linuxdeepin/dde-control-center/commit/ab908a1727adde3a889085aeb7d192165332c7b4))

#### Features

*   disable second page create ([5821e270](https://github.com/linuxdeepin/dde-control-center/commit/5821e270e7f1211e346859f7a3e5934abf98ae64))
*   real time sync navigation bar ([a64cbc17](https://github.com/linuxdeepin/dde-control-center/commit/a64cbc17135351acf4cf330cfa34adb1cc356cdf))
*   optimize animation ([964d324f](https://github.com/linuxdeepin/dde-control-center/commit/964d324ffe95689f108f76f2814125dc953619cf))
*   check disk space for update ([29acc794](https://github.com/linuxdeepin/dde-control-center/commit/29acc7941c0b38b84d2e0d5704e2f29eb0a1b224))
*   move auto-exit settings to gsettings ([c11c1da4](https://github.com/linuxdeepin/dde-control-center/commit/c11c1da4c9c80317fcdbd918f96210dcced4df2f))
*   encrypt password in dbus transfer ([5cac53d6](https://github.com/linuxdeepin/dde-control-center/commit/5cac53d615cab91603f5106fcba4002bba64e0c5))
* **bluetooth:**
  *  disable discover when leave bluetooth page ([6c953772](https://github.com/linuxdeepin/dde-control-center/commit/6c953772eea568a3913a81e9cc6bef0d2b975772))
  *  support close pincode dialog ([47f019bf](https://github.com/linuxdeepin/dde-control-center/commit/47f019bfc79128dce9ca5148e690d194083eea0d))
* **network:**  hide export button in vpn create page ([2361a9aa](https://github.com/linuxdeepin/dde-control-center/commit/2361a9aa605cfad5dec31981e6e61ed3e3765caa))
* **weather:**  add log ([e00d53ef](https://github.com/linuxdeepin/dde-control-center/commit/e00d53efdafa9cd8c8294e9bafd0d485f6f142ce))



<a name="4.4.4"></a>
### 4.4.4 (2018-03-28)


#### Bug Fixes

* **network:**  wireless connection device info not saved ([31be9421](https://github.com/linuxdeepin/dde-control-center/commit/31be94218dca3855fb70ca5f86cd0aad73958349))
* **notify:**  use time as id ([c9b984b6](https://github.com/linuxdeepin/dde-control-center/commit/c9b984b6a3a75e930362b91c4377db239eb86f6c))

#### Features

*   reactive connection when settings changed ([af3ca20c](https://github.com/linuxdeepin/dde-control-center/commit/af3ca20c5e2475e00a81e3b433cb9ff1202d72d9))



<a name="4.4.3"></a>
### 4.4.3 (2018-03-26)


#### Bug Fixes

* **grub:**  error hide frame when update grub background ([86c68180](https://github.com/linuxdeepin/dde-control-center/commit/86c681803c4daed22f5d6bdbfceb70c91946c4ac))
* **network:**  Using UUID as a condition ([7ab70303](https://github.com/linuxdeepin/dde-control-center/commit/7ab70303e1f8f6d7d5c368c09141f9f50b90d9b2))
* **update:**  Error checking low battery ([9c2f944c](https://github.com/linuxdeepin/dde-control-center/commit/9c2f944cbabb42c7b35f17d39be37dee8f0b2243))



<a name="4.4.2"></a>
### 4.4.2 (2018-03-22)


#### Bug Fixes

* **accounts:**
  *  restart lwsmd before join AD Domain ([b25ccb51](https://github.com/linuxdeepin/dde-control-center/commit/b25ccb515e045e3c468d324220299b4270ac1a5f))
  *  QProcess exitcode error when join AD Domain ([face4c81](https://github.com/linuxdeepin/dde-control-center/commit/face4c81b6eac6fba8bef8264c7c338c44e97add))
* **display:**  move position error when resolution changed ([3a8f7c84](https://github.com/linuxdeepin/dde-control-center/commit/3a8f7c84ccaa2f7ca0905159a59c5e842c250afe))
* **notify:**  title not limit on width ([ddd1b2c6](https://github.com/linuxdeepin/dde-control-center/commit/ddd1b2c6e3dbc6972e2265fdb0dfee3147584a9e))



<a name="4.4.1"></a>
### 4.4.1 (2018-03-16)


#### Features

*   reminder dialog icon support HiDPI ([3ab10f7c](https://github.com/linuxdeepin/dde-control-center/commit/3ab10f7c3415dc71a907d672f5b7e3b331814e14))
* **accounts:**  AD Dialog add error tips ([e3f27b4e](https://github.com/linuxdeepin/dde-control-center/commit/e3f27b4e753b5763c7156ebbdb4b6edb3c9c7a18))
* **update_module:**  result item icon support HiDPI ([ccaecc85](https://github.com/linuxdeepin/dde-control-center/commit/ccaecc85e906bd918617aa973c2223cc399855ed))

#### Bug Fixes

*   not sync navigation ([5cba4210](https://github.com/linuxdeepin/dde-control-center/commit/5cba421069269f3bdf246d364a867a5c010b0a63))
*   wireless list icon support HiDPI ([10d2243c](https://github.com/linuxdeepin/dde-control-center/commit/10d2243c6562207df57745331d4d6ba4036dafe9))
*   close ADDialog not hidden. ([2b57a334](https://github.com/linuxdeepin/dde-control-center/commit/2b57a3344e192f85e690a0cbbd90a1bfd6011aa1))
*   QuickControlPanel not set a valid parent for child widget ([3a364a93](https://github.com/linuxdeepin/dde-control-center/commit/3a364a934e1acaedab69f2a6807b6bf287934727))
* **accounts:**
  *  check AD User logind ([4f92dcbe](https://github.com/linuxdeepin/dde-control-center/commit/4f92dcbe7fcac5acbb65859a6a4f2b81ea3e6369))
  *  join AD Domain not set init value ([1aa5e463](https://github.com/linuxdeepin/dde-control-center/commit/1aa5e463af2f898b5561f5db4cc242e65c279ac4))
* **display:**  screen list item margins error ([0cc5c700](https://github.com/linuxdeepin/dde-control-center/commit/0cc5c700b476e7693df10f02933082dac34a0c9f))
* **keyboard:**  set fix height need delay when item set title ([3974c122](https://github.com/linuxdeepin/dde-control-center/commit/3974c1224697411c1962dce7ee3fc35dd0b72262))



<a name="4.4.0"></a>
## 4.4.0 (2018-03-08)


#### Bug Fixes

*   crash when user avatars count less than 14 ([4bd05eea](https://github.com/linuxdeepin/dde-control-center/commit/4bd05eeaeafd9187069a027c3021af5ffb5b496f))
*   using the wrong macro ([742e5806](https://github.com/linuxdeepin/dde-control-center/commit/742e5806948e718def1040f7e2b6f54e36c7d3b6))
*   installer set vendor name as latin1 ([7d53e35c](https://github.com/linuxdeepin/dde-control-center/commit/7d53e35ceda43b0304beb704eeaee5a250889221))
*   tips align left 16px ([f8ac2c62](https://github.com/linuxdeepin/dde-control-center/commit/f8ac2c6277a3d21155a7cc890d3de18f389486fd))
*   MPRIS play button icon not using big pix in hidpi ([774b82dc](https://github.com/linuxdeepin/dde-control-center/commit/774b82dc3936b6cf47c03d1e2818511005387d66))
* **account:**
  *  AD Dialog style error ([d482d7f9](https://github.com/linuxdeepin/dde-control-center/commit/d482d7f93a4e85b795432cb75c064fcce89ecb58))
  *  Button status error after cancelling authentication ([4c3769c6](https://github.com/linuxdeepin/dde-control-center/commit/4c3769c615ae4bde7ec4732fca714833d470eb9a))
* **accounts:**  back when change fullname successed ([ab18612f](https://github.com/linuxdeepin/dde-control-center/commit/ab18612f59538f4762531ee73ee6384c5f72a3f5))
* **datetime:**  timezone list is empty, group head is visible ([9fb38e53](https://github.com/linuxdeepin/dde-control-center/commit/9fb38e5350f556fccf4d6b4eb0a73f39d6bbcc9a))
* **dbus:**  ToggleInLeft handle Toggle ([13671739](https://github.com/linuxdeepin/dde-control-center/commit/13671739abd477c4b42ac31f195af7b2924d011d))
* **defapp:**
  *  not create local folder ([3c9ee223](https://github.com/linuxdeepin/dde-control-center/commit/3c9ee223128da4bf197802e4517fc02a5fbfe46d))
  *  not use deepin-custom head ([2ad8b3ff](https://github.com/linuxdeepin/dde-control-center/commit/2ad8b3ff612bb8d5c3cf8f40f2ad8c6127d47772))
* **display:**
  *  primary screen is error in monitors ground ([52d1e65a](https://github.com/linuxdeepin/dde-control-center/commit/52d1e65a15beff8b1b60dbf83b3c8dd337f0dabb))
  *  current resolution not select ([23d4e2ac](https://github.com/linuxdeepin/dde-control-center/commit/23d4e2ac10016bca3498d50fda7b7f457d104852))
* **frame:**
  *  update notifier pos error ([a4819eae](https://github.com/linuxdeepin/dde-control-center/commit/a4819eaeb26f39c49f20ffe249201a34490de871))
  *  No navigationbar is updated after entering the module ([38f7060f](https://github.com/linuxdeepin/dde-control-center/commit/38f7060f8b45640a3795dbc5d22bc2c2030211c1))
  *  updatenotifier not wordwrap ([7939d0bc](https://github.com/linuxdeepin/dde-control-center/commit/7939d0bc8f6991789b76dd18d573a70f0e1c7ff3))
  *  not reset position when hideImmediately ([b51ae87b](https://github.com/linuxdeepin/dde-control-center/commit/b51ae87bab3e18c7ff8de728a07749485b969c50))
* **grub:**
  *  The width is too wide after the navigation bar is added ([8101cb94](https://github.com/linuxdeepin/dde-control-center/commit/8101cb94c73e51a07a497fb7334a4a0b8d9914ff))
  *  early call leads to failure ([c5700275](https://github.com/linuxdeepin/dde-control-center/commit/c57002751a64f4ebec5d8b102deb59b7f2ebee13))
* **keyboard:**
  *  The width is too wide after the navigation bar is added ([cf98272b](https://github.com/linuxdeepin/dde-control-center/commit/cf98272b4f98b5fe9679406eb763c71a8be20732))
  *  delay load data cause of DBus is not ready ([45c1c414](https://github.com/linuxdeepin/dde-control-center/commit/45c1c414ddcd37cc27dc73491fb6704cbb3e1122))
  *  search timer not set single mode ([3b99a71f](https://github.com/linuxdeepin/dde-control-center/commit/3b99a71f738b8ba818a777d7bc82694815f35c07))
* **mouse:**
  *  lack of set values of touchpad ([734d6586](https://github.com/linuxdeepin/dde-control-center/commit/734d6586fd3cbe924a789c229198734ab4001d8d))
  *  hide palm detect when touchpad is not exist ([b230501f](https://github.com/linuxdeepin/dde-control-center/commit/b230501fc9b5eaef7e1a65b8b1b74beb64e404d2))
  *  hide disable touchpad setting when touchpad is not exist ([82f79609](https://github.com/linuxdeepin/dde-control-center/commit/82f796097ff12c7c666dcbe7d13e73250f44702e))
  *  error touchpad visible value ([64672d52](https://github.com/linuxdeepin/dde-control-center/commit/64672d529055686351f039e076db91e72f7fb342))
* **navgation:**  Add compile macro ([f9e1e2ad](https://github.com/linuxdeepin/dde-control-center/commit/f9e1e2ad81caefed90c61e630517670341a6b159))
* **navigation:**  The module does not exist and does not display ([403241ed](https://github.com/linuxdeepin/dde-control-center/commit/403241ed3dc212c34821d0a1b85a2fd8bb618b8a))
* **network:**
  *  hide appproxy when proxychains is not exist ([c92333a4](https://github.com/linuxdeepin/dde-control-center/commit/c92333a4f83b49fd4dd3deec3501911b99493e4e))
  *  crash of network device changed ([696ca94e](https://github.com/linuxdeepin/dde-control-center/commit/696ca94e372d6f3db7c79fdafa1f46ace8f2c566))
* **notify:**
  *  reinvent data ([29cb26b0](https://github.com/linuxdeepin/dde-control-center/commit/29cb26b0136d9f2ef5942ad1a174e6e121b71fdc))
  *  update item width ([e5298861](https://github.com/linuxdeepin/dde-control-center/commit/e5298861a8b8e87ccc8cd740e249b2a255ae3a2d))
* **personalization:**  not load the data when animation is not finished ([ad024556](https://github.com/linuxdeepin/dde-control-center/commit/ad0245565458bc23837261baf11993e11a028749))
* **sound:**  item repeat select ([2dcc01a8](https://github.com/linuxdeepin/dde-control-center/commit/2dcc01a8e1d41f3f79e096201d8c2661d5acab8c))
* **systeminfo:**  advance start grub2 ([78e27cca](https://github.com/linuxdeepin/dde-control-center/commit/78e27cca124cb8370c49f1bf2cddfbcd6c3bb871))
* **time:**  other timezone not update when change current timezone ([ed3a6fbe](https://github.com/linuxdeepin/dde-control-center/commit/ed3a6fbebba01308bd198b62a368e469e3a11714))
* **update:**
  *  disable source mirror when system type is Professional ([2dc4d015](https://github.com/linuxdeepin/dde-control-center/commit/2dc4d015535c6fc70917341d5d636939c1d513b0))
  *  low power can update system ([a8706293](https://github.com/linuxdeepin/dde-control-center/commit/a87062930d0ae59c24e15d2910d7a45a511493b1))
  *  repair cannot update when resotre power ([32fa63ce](https://github.com/linuxdeepin/dde-control-center/commit/32fa63ce963a7a32613bb4f1040851137d575445))
  *  summary not newline ([f88aed7c](https://github.com/linuxdeepin/dde-control-center/commit/f88aed7ca5014db65f8391e840eb99d269ba5c80))
  *  updated mode error ([2ac2795c](https://github.com/linuxdeepin/dde-control-center/commit/2ac2795c5c2b2cf261a186ec6a938db3ee17e1bc))
  *  not call reboot-reminder-dialog ([1951f7cb](https://github.com/linuxdeepin/dde-control-center/commit/1951f7cb58657f990dfe83c0409d16412a389a58))
* **weather:**  location button not align ([05a4629b](https://github.com/linuxdeepin/dde-control-center/commit/05a4629b060d4984851152acdb5d15696247ebf5))

#### Features

*   enable ad domain for professional version ([cc8ca447](https://github.com/linuxdeepin/dde-control-center/commit/cc8ca4472906dfb5ebf77584a5b53c9035f0ab6e))
*   update wmswitcher interface ([3ec49a35](https://github.com/linuxdeepin/dde-control-center/commit/3ec49a35d07d8ccf227e2331b490e5845f2e52fb))
* **account:**  Support the prohibition of modifying the password of other users ([b39bb594](https://github.com/linuxdeepin/dde-control-center/commit/b39bb5947abbaa7a09de21ab18cea3ab5bd6b013))
* **accounts:**
  *  crypt user password when create a new user ([686d915a](https://github.com/linuxdeepin/dde-control-center/commit/686d915ad873388b5f1e6d885a3b4c1cf1498ae0))
  *  use passwd to change user passwd ([e4b0a9c5](https://github.com/linuxdeepin/dde-control-center/commit/e4b0a9c586f0fda8439136c80a27ebff25ef8c51))
  *  add addomain policy and notify ([12ee7c5a](https://github.com/linuxdeepin/dde-control-center/commit/12ee7c5ae977081dcc8831094942d2997f531d02))
  *  add AD Domain dialog ([291dad21](https://github.com/linuxdeepin/dde-control-center/commit/291dad215912cbcc429fbb895605483c896edce1))
* **mouse:**
  *  support change wheel speed ([bcb4ebfb](https://github.com/linuxdeepin/dde-control-center/commit/bcb4ebfb8b6944bec5b6efebb49646148a4c46d7))
  *  add palm detect ([28c66974](https://github.com/linuxdeepin/dde-control-center/commit/28c669744affdd78d46bbf74b3a6ae86a60f3400))
* **network:**  allow modify network settings when cable not pluged ([3259539a](https://github.com/linuxdeepin/dde-control-center/commit/3259539a7ad6b44fec6de22ba4fda98f131bb29f))
* **update:**
  *  check netselect exist ([5cac464e](https://github.com/linuxdeepin/dde-control-center/commit/5cac464e432dd04365358a139e1425640f6b17f6))
  *  add disable source check macro ([41e2c461](https://github.com/linuxdeepin/dde-control-center/commit/41e2c461a4a3b3eea89eec2c2f6f7a50a88909d4))
  *  support source list check ([656511cf](https://github.com/linuxdeepin/dde-control-center/commit/656511cf8e9a54bdfefd7a73f8645f05d817c63c))
  *  Use progress bar instead of animation ([c530e36b](https://github.com/linuxdeepin/dde-control-center/commit/c530e36bde7d015b044d01469da088fe2417a7ce))
  *  no updates when no network ([dfb8a853](https://github.com/linuxdeepin/dde-control-center/commit/dfb8a8535bde709258e636175d93a6520dbdb159))



<a name="4.3.7"></a>
### 4.3.7 (2017-11-29)


#### Bug Fixes

* **update:**  not update checkupdate status ([b24d20e2](https://github.com/linuxdeepin/dde-control-center/commit/b24d20e26f895c1a71c7dcda1af37cf5477c8c37))



<a name="4.3.6"></a>
### 4.3.6 (2017-11-28)


#### Bug Fixes

* **display:**  initializing value ([85fd606b](https://github.com/linuxdeepin/dde-control-center/commit/85fd606bb35fea1c992ed573f537f4a4fb9a071d))
* **network:**  update select svg ([40bd3033](https://github.com/linuxdeepin/dde-control-center/commit/40bd30331ef953fd49d4f964ebf08b645d4854d5))



<a name="4.3.5"></a>
### 4.3.5 (2017-11-24)


#### Bug Fixes

* **account:**  user name is empty when display name changed ([bbfd9d1d](https://github.com/linuxdeepin/dde-control-center/commit/bbfd9d1d182e5fd96b3ed86e0492eaef0639545f))
* **display:**  replace DImageButton to QLabel of brightnessitem ([32d4f50e](https://github.com/linuxdeepin/dde-control-center/commit/32d4f50e68e40116d70321f8df23fa52512d22df))



<a name="4.3.4"></a>
### 4.3.4 (2017-11-23)


#### Features

* **network:**  add show password button ([6918a66f](https://github.com/linuxdeepin/dde-control-center/commit/6918a66fb31cfe84aa8261f57715e421410d21f6))
* **widget:**  add passwdeditwidget ([ae6fdb6b](https://github.com/linuxdeepin/dde-control-center/commit/ae6fdb6bdee3b7f65b235188b9b772463ec226cb))

#### Bug Fixes

* **file:**  pics permission error ([1abe5dae](https://github.com/linuxdeepin/dde-control-center/commit/1abe5dae812a2d9421dc1d6539d3d86b257a7d39))
* **key:**  search key disordered layout ([5d1677d4](https://github.com/linuxdeepin/dde-control-center/commit/5d1677d449e9b55975a9b9f0f498558d13d574e7))



<a name="4.3.3"></a>
### 4.3.3 (2017-11-16)


#### Bug Fixes

* **display:**
  *  recognize dialog position error ([1d873ddd](https://github.com/linuxdeepin/dde-control-center/commit/1d873ddd525b8aa67d07f0dcdb44d8720a7a216c))
  *  crash when resolution not in support list ([5d01fcac](https://github.com/linuxdeepin/dde-control-center/commit/5d01fcacb598ce5b0e6ed8b6afb7bfaa50230c0e))
* **frame:**
  *  not refresh allsettings page ([e6de74e4](https://github.com/linuxdeepin/dde-control-center/commit/e6de74e486f2337aee5ae83d988fca2bff79623a))
  *  topleft pos error ([08485872](https://github.com/linuxdeepin/dde-control-center/commit/08485872d8ea391e60e2202c4ce0f84ccbd3ff6b))
  *  update nav icon file ([847cdc7c](https://github.com/linuxdeepin/dde-control-center/commit/847cdc7ca29f8c9915cb90216935d576557823f3))
  *  can not show again when call hideimmediately ([b4befc98](https://github.com/linuxdeepin/dde-control-center/commit/b4befc980f41f7a87b107aaed3b2a11894fcd32d))
* **proxy:**  application proxy check ip address legal ([890b33f8](https://github.com/linuxdeepin/dde-control-center/commit/890b33f8e37c8efd063819494aa44d53f7b31b73))
* **sound:**  repeat select active port ([fc192c3f](https://github.com/linuxdeepin/dde-control-center/commit/fc192c3fa147c0351ded5798cfa935b884ab4303))
* **update:**
  *  check updates when downloaded ([e4ee2877](https://github.com/linuxdeepin/dde-control-center/commit/e4ee2877581b85cf7de09310cac2b54795951534))
  *  error install when auto download open ([043656b4](https://github.com/linuxdeepin/dde-control-center/commit/043656b4b55d3c0a31919ea4248a8eb5d59f829f))

#### Features

* **accounts:**  show fullname ([b92008a7](https://github.com/linuxdeepin/dde-control-center/commit/b92008a793c77b178eeae81fe60567200e86564a))
* **personalization:**  update font dbus interface ([9fa3f23d](https://github.com/linuxdeepin/dde-control-center/commit/9fa3f23df99bf134d2e6fdec0a1578a936772bda))



<a name="4.3.2"></a>
### 4.3.2 (2017-11-09)


#### Bug Fixes

*   hotspot switch state error ([1db9d590](https://github.com/linuxdeepin/dde-control-center/commit/1db9d590aa66899b7482ca95f2d5b52294d41f6e))
* **display:**  not refresh night mode status ([2f636efd](https://github.com/linuxdeepin/dde-control-center/commit/2f636efd92cb51da3ef9bc720b1b8fadf34d2ed5))
* **fprint:**  advance stop enrollstop ([b71ecf1e](https://github.com/linuxdeepin/dde-control-center/commit/b71ecf1e1578ccfdc0e8af9a7e4f8ad841285161))
* **frame:**  use DRegionMonitor ([78d237b3](https://github.com/linuxdeepin/dde-control-center/commit/78d237b32884142c5936e131f9ecf13f96c7d02f))
* **module:**  return that settings page are not show ([b84a5c3b](https://github.com/linuxdeepin/dde-control-center/commit/b84a5c3b505c22f9a978a587871ac6dea295713d))
* **network:**  wireless typo ([7674fb13](https://github.com/linuxdeepin/dde-control-center/commit/7674fb1337fb41907abaa7139902b932b406fe23))
* **sound:**  modify the frequency of calling tick ([e083848f](https://github.com/linuxdeepin/dde-control-center/commit/e083848f1aed7c3d3c93ab41955c5d96c9d2d1b7))
* **update:**  not clean failed jobs ([3cac1162](https://github.com/linuxdeepin/dde-control-center/commit/3cac116201a03e05369e8290dc58a94bd61d689b))
* **weather:**  null pointer check ([a62c40dd](https://github.com/linuxdeepin/dde-control-center/commit/a62c40dd7ccd5974d7cad3df7d6cd63c53834003))

#### Features

* **network:**  add vpn connecting animation ([fea78421](https://github.com/linuxdeepin/dde-control-center/commit/fea78421cbfccfd19964605e6fed492d59636444))



<a name="4.3.1"></a>
### 4.3.1 (2017-11-03)


#### Features

*   support hide Index page ([c4ab64fa](https://github.com/linuxdeepin/dde-control-center/commit/c4ab64faa85fcf007771bb6f6b3287f7973120b3))
* **fprint:**
  *  add retry when scan failed ([96721b63](https://github.com/linuxdeepin/dde-control-center/commit/96721b638b44a3caa227514e4eb721948347a93f))
  *  use new animation ([f5d496c2](https://github.com/linuxdeepin/dde-control-center/commit/f5d496c2cea0325c8698f1a4b8f5a071a410ac64))
* **grub:**  getbackground use new dbus interface for grub theme ([6c317115](https://github.com/linuxdeepin/dde-control-center/commit/6c317115f1c243945e6155eddece0d5d1977ae4a))
* **network:**
  *  add new compilers condition ([0bec67c3](https://github.com/linuxdeepin/dde-control-center/commit/0bec67c3f393ccfc13acb8de093719eda90c7a65))
  *  support display info with cloned MAC addr ([16ce3b7f](https://github.com/linuxdeepin/dde-control-center/commit/16ce3b7f7cf0f6113e3252a927fed9847206ee7b))
  *  hide ap list when device in AP mode ([22b536f7](https://github.com/linuxdeepin/dde-control-center/commit/22b536f7790a41ffd31b84e965613bc4b4159df2))
* **shortcut:**  add system-monitor and color-picker ([1fa2940a](https://github.com/linuxdeepin/dde-control-center/commit/1fa2940a9f5941c243b06dda7706d146ea48fdc1))

#### Bug Fixes

*   change redshift to forced dependency ([a57fee9b](https://github.com/linuxdeepin/dde-control-center/commit/a57fee9b5262d2b8fb9b9cac8d36853abd584c9a))
*   error setting auto hide ([b52c334c](https://github.com/linuxdeepin/dde-control-center/commit/b52c334c3c396207fb22f8b0a4edba52d0d2b948))
* **TipsItem:**  support multiple lines ([095cde5b](https://github.com/linuxdeepin/dde-control-center/commit/095cde5b80d056be3ec1ae9ed5e74cfef58ec90e))
* **finger:**  cannot add finger ([cbc6e3cb](https://github.com/linuxdeepin/dde-control-center/commit/cbc6e3cbcd5a3aeeab2f74c8a3e676038873a5d1))
* **fprint:**  remove useless labmda ([5a040006](https://github.com/linuxdeepin/dde-control-center/commit/5a0400060611ec32f1369ee719454457ee727fe8))
* **hidpi:**  widget overlap ([5036a6a8](https://github.com/linuxdeepin/dde-control-center/commit/5036a6a836c741840c787aa60498d9e537b59757))
* **keyboard:**  linker error when set DISABLE_KEYBOARD=YES ([85a482f3](https://github.com/linuxdeepin/dde-control-center/commit/85a482f3a1d4f064e15765756a141937070b3a9b))
* **network:**  hotspot switch state error ([1c76bdb6](https://github.com/linuxdeepin/dde-control-center/commit/1c76bdb685d33ff1bcb86e6555c7c4c77a06fc0a))
* **shortcut:**
  *  not save custom shortcut info ([fff33e4f](https://github.com/linuxdeepin/dde-control-center/commit/fff33e4f20762306c494753b0bc3606f56ee6f13))
  *  shortcut list keep the order ([6845eb00](https://github.com/linuxdeepin/dde-control-center/commit/6845eb0084af9a93b364b4c163ac68e1aacb86cb))
  *  remove old's shortcutkey ([2de0543e](https://github.com/linuxdeepin/dde-control-center/commit/2de0543ead0cf857989201c78fab865c31dbdbe0))
  *  remove customitem's operation on DBus ([cd72e6e7](https://github.com/linuxdeepin/dde-control-center/commit/cd72e6e779eeca6217d29baeceda014f76c0241b))
  *  remove shortcutitem's operation on DBus ([d08760e4](https://github.com/linuxdeepin/dde-control-center/commit/d08760e4201965c3cf5d1b278b246bc10a06a75d))
* **sysinfo:**  boot menu icon error on hidpi ([c745d4c0](https://github.com/linuxdeepin/dde-control-center/commit/c745d4c021a4088ef48c1393536b0dc336e6eadb))
* **update:**  not set failed when refresh failed ([3717c340](https://github.com/linuxdeepin/dde-control-center/commit/3717c34098bfb2a94fc75be68ebef912e45d3375))
* **widgets:**  rename for edit icon ([d8939ffa](https://github.com/linuxdeepin/dde-control-center/commit/d8939ffa25df75a8dea1b11df05775e920ea9914))



<a name=""></a>
##  4.3 (2017-10-27)


#### Bug Fixes

*   keyboard custom item key hide when lost focus ([7decabcc](https://github.com/linuxdeepin/dde-control-center/commit/7decabcc6fe0e74ec09930dcfe2dd8d418ea867e))
*   night mode not check service is valid ([916ad375](https://github.com/linuxdeepin/dde-control-center/commit/916ad37524e2a923c8ed70911746ba9131a95d18))
*   update redshift to suggest ([a1695d7c](https://github.com/linuxdeepin/dde-control-center/commit/a1695d7cb55303ed77c624262e1c4aa88ce8db3c))
*   not check finger device is valid ([5abd2569](https://github.com/linuxdeepin/dde-control-center/commit/5abd2569c33f04cc9ba5f1de2d0451744a64c1a4))



<a name=""></a>
## 4.2.5.12 (2017-10-26)


#### Bug Fixes

*   redshift not disable boot when close ([a4de37a7](https://github.com/linuxdeepin/dde-control-center/commit/a4de37a74f0b14e0db2f8c89811ed0be4f439b76))
*   correct header filename ([889d24ab](https://github.com/linuxdeepin/dde-control-center/commit/889d24ab2f66376b81307066bba1509bf0e12c23))



<a name=""></a>
## 4.2.5.11 (2017-10-25)


#### Bug Fixes

*   add dependencies and enable services ([70aad405](https://github.com/linuxdeepin/dde-control-center/commit/70aad405132096414b2319722d0feeed37a477d5))
*   finger ts error ([91ca4432](https://github.com/linuxdeepin/dde-control-center/commit/91ca44327e7fc52a10d08c401e64cb90b8f08a0a))
*   chainsproxy spelling error ([e43084ff](https://github.com/linuxdeepin/dde-control-center/commit/e43084ff84b2d5d70265ca4e4a15c58cbd58aa52))
*   shortcut item title not align left ([f1e7c7ca](https://github.com/linuxdeepin/dde-control-center/commit/f1e7c7cac4558a8c68f529907c5a8572e08ec600))
*   not set auto hide when set lang ([660f5c02](https://github.com/linuxdeepin/dde-control-center/commit/660f5c0221e41000de83d2292c246f9c221a4b16))
*   custom item use shortcut label ([2f46a3e8](https://github.com/linuxdeepin/dde-control-center/commit/2f46a3e8895aff8fe0d44785766fd4d0b33d2d53))
*   shortcut label not width ([478b95e1](https://github.com/linuxdeepin/dde-control-center/commit/478b95e133fc5d6aa6aa01c249b540dd3cbb21e7))
*   set nopasswd login will not auto hide frame ([bf1783ba](https://github.com/linuxdeepin/dde-control-center/commit/bf1783ba915422dd7da333f853e0222d301d0580))
*   change date will not auto hide of frame ([0b708caf](https://github.com/linuxdeepin/dde-control-center/commit/0b708caf0db3b094e0f6cf78dc947caaeebf6e78))
*   keyboard add display key name ([f062c5e9](https://github.com/linuxdeepin/dde-control-center/commit/f062c5e9746a5def1efa5b5c086f6f70175911ec))
*   change boot config will not auto hide of control center ([31df6eab](https://github.com/linuxdeepin/dde-control-center/commit/31df6eabb4e17614a16d42eed7c2a616908f7560))
*   update mirror select svg update ([bda35959](https://github.com/linuxdeepin/dde-control-center/commit/bda3595993061a1254cd8a6c65c97468775c00f2))
*   conflict shortcut is not on the shortcut list ([d5a2aa55](https://github.com/linuxdeepin/dde-control-center/commit/d5a2aa55bc7e6c4c5f7e2170be3f62772c05ebab))
*   quick control select not update ([bd5d05ad](https://github.com/linuxdeepin/dde-control-center/commit/bd5d05addd78e4aa8b46b65529bd8d9c0ab1f648))
*   not update shortcut when shortcut changed ([0bb1faa0](https://github.com/linuxdeepin/dde-control-center/commit/0bb1faa0aa8bd0614b85b26832c0df7e938462c6))
*   systeminfo module compile parameter location error of longxin ([d29d9604](https://github.com/linuxdeepin/dde-control-center/commit/d29d96047547ffe97bdbabee895442608b0bca23))
*   change copywriting of Proxy to System Proxy ([b73a1579](https://github.com/linuxdeepin/dde-control-center/commit/b73a1579b4d0cd497076a4acb70b0ba82133b852))
*   datetime zone item not use svg file ([d8871e3f](https://github.com/linuxdeepin/dde-control-center/commit/d8871e3f9b55531527b60fbb631e7ba72482105d))
*   refresh list when appstore installed ([cb6c2c2d](https://github.com/linuxdeepin/dde-control-center/commit/cb6c2c2d351d83cf8b0dce1c845ad4542ea6386f))
*   keyboard select and remove icon not support hidpi ([0e4ca423](https://github.com/linuxdeepin/dde-control-center/commit/0e4ca423da711a7270517640f388579a35a86575))
*   nextbutton normal svg file error ([8976565f](https://github.com/linuxdeepin/dde-control-center/commit/8976565f18df5c0b65d231f4755d16c41daac920))
*   display module compile parameter location error of longxin ([176a8e95](https://github.com/linuxdeepin/dde-control-center/commit/176a8e95c1b2e8a944d3e1fd81d19b770a01e43a))
*   network proxy struct initialization error ([7387ca77](https://github.com/linuxdeepin/dde-control-center/commit/7387ca774046b094b9e89f2898fa40e04e8e2b54))
*   display module compile parameter location error of longxin ([e928f0e0](https://github.com/linuxdeepin/dde-control-center/commit/e928f0e0504a38620fbd902c76c77da36281d4c8))
*   system shortcut key remove useless filter ([19770b39](https://github.com/linuxdeepin/dde-control-center/commit/19770b3901f88d07ebf43263bd6d48b3b514bae8))
*   custom edit item draw not replace text ([7eb0225a](https://github.com/linuxdeepin/dde-control-center/commit/7eb0225aff3211aa6ab39102bbc440e84baec6d2))
*   update not set failed status ([ad2ec69e](https://github.com/linuxdeepin/dde-control-center/commit/ad2ec69edfe8211820c580461b99e2908593a54e))
*   display module use dbus change sacle ([39fd531d](https://github.com/linuxdeepin/dde-control-center/commit/39fd531d479c9f8d199b20390352a2183c57699c))
*   remove lateency hide ([01f14827](https://github.com/linuxdeepin/dde-control-center/commit/01f148279a98b67d1b0d806a17ae7b31a6505571))
*   wired network choose file not set frame autohide ([120d060f](https://github.com/linuxdeepin/dde-control-center/commit/120d060fc3d7b5adb04e31a1daedbe0ea9de1f7e))
*   notify remove useless png and correct scale icons ([386c4b7a](https://github.com/linuxdeepin/dde-control-center/commit/386c4b7a418a8bb9eb5d5b15f3af5038b1055b4f))
*   update module incorrect progress ([be7fe066](https://github.com/linuxdeepin/dde-control-center/commit/be7fe0661afd652f028261064b01c3a34722cd33))
*   disable QLineEdit right menu ([dac52357](https://github.com/linuxdeepin/dde-control-center/commit/dac52357f5dfa1048364823374a7aad8a1fe6316))
*   mouse without view also has hover status ([a0eff771](https://github.com/linuxdeepin/dde-control-center/commit/a0eff771eaa57e0716c4d472140ece8667a0f13d))
*   reboot reminder dialog not window shadow ([54d73ebc](https://github.com/linuxdeepin/dde-control-center/commit/54d73ebcc23c3a0a8e208b6c8c7db52a6ee70e29))
*   navgation not icon when not scale ([e94e4e14](https://github.com/linuxdeepin/dde-control-center/commit/e94e4e1497fe2df84433deff353088ba5b8d665d))
*   support multiple SVG files ([1251b494](https://github.com/linuxdeepin/dde-control-center/commit/1251b494656a0d4932c2486074b71bdfc91d6995))
*   size error when qt scale ([77222f37](https://github.com/linuxdeepin/dde-control-center/commit/77222f37542bfca69a781da70d04c30ad7e9c4bd))
*   navitem icon not align center ([34b62ea6](https://github.com/linuxdeepin/dde-control-center/commit/34b62ea688dc1c8a2ed65612d3f357c0ff42ac76))
*   bluetooth not sync status ([32995ba0](https://github.com/linuxdeepin/dde-control-center/commit/32995ba02898aa61958e68fed7ef0316242f6cc3))

#### Features

*   add finger page ([d4e940b3](https://github.com/linuxdeepin/dde-control-center/commit/d4e940b3c48c621b1164abc258048bcbdb7ba801))
*   add finger item ([d341deac](https://github.com/linuxdeepin/dde-control-center/commit/d341deac54eccbd623b57cf5253bf0e215836eb2))
*   add fprintf dbus ([647b1070](https://github.com/linuxdeepin/dde-control-center/commit/647b1070f29ce42a7c2982da4995403fd961beb1))
*   font next widget show font name ([d79811dd](https://github.com/linuxdeepin/dde-control-center/commit/d79811dd5411e430e47ddc39992afa6c7d82bd54))
*   accounts add finger widget ([02e8ccc2](https://github.com/linuxdeepin/dde-control-center/commit/02e8ccc26ea2f8d592a41b9990ceec36fb369cf2))
*   add night mode ([ce737e1c](https://github.com/linuxdeepin/dde-control-center/commit/ce737e1ca7e4c50eed2e27f48ae6aee211e15d29))
*   update loading @2x files ([31a58f79](https://github.com/linuxdeepin/dde-control-center/commit/31a58f796c39ae07c6720a707d36f58f4d826e97))
*   accounts user support nopasswd login ([a03f13b4](https://github.com/linuxdeepin/dde-control-center/commit/a03f13b4212044f179d23a5b8d589e202bdc2591))
*   add avatar button support hidpi ([3580541b](https://github.com/linuxdeepin/dde-control-center/commit/3580541b874fe05c84404888dea001c6d6a715dc))
*   weather support hidpi ([a8b69edd](https://github.com/linuxdeepin/dde-control-center/commit/a8b69edd931a3345f565c2b246e8964a68678985))
*   rotate support hidpi ([a2ddd930](https://github.com/linuxdeepin/dde-control-center/commit/a2ddd930e0c77eefdee3ee11ff6bff1b66facc2b))
*   boot module support hidpi ([b7b980aa](https://github.com/linuxdeepin/dde-control-center/commit/b7b980aa0ffb28e445d377120b7100b6ee95c76f))
*   quick bluetooth support hidpi ([beb2646b](https://github.com/linuxdeepin/dde-control-center/commit/beb2646be2a52f57f03a95654491e54043b37d82))
*   keyboard module support hidpi ([1b2f480c](https://github.com/linuxdeepin/dde-control-center/commit/1b2f480c69ba12d4b11ab1afde43622659c21cdc))
*   accounts support set nopasswd login ([32021317](https://github.com/linuxdeepin/dde-control-center/commit/32021317e4fb29459d85d096056a16e66793cdef))
*   update keybinding interface ([7ecf59ba](https://github.com/linuxdeepin/dde-control-center/commit/7ecf59baa46e2aab9df9b341977377f0ef379102))
*   quick wifi support hidpi ([d1f03560](https://github.com/linuxdeepin/dde-control-center/commit/d1f03560414fb8f0cd4d9fa1271e131ee536ec46))
*   display support adjust window scale ([5f79ab9b](https://github.com/linuxdeepin/dde-control-center/commit/5f79ab9bc7f9093cabda71e901e5d089903ffebb))



