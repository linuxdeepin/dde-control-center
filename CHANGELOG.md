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



