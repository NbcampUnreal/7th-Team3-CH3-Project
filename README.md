🎮 [챕터3 3조 팀프로젝트] 그림자의 관

[여기에 게임의 한 줄 소개를 적어주세요. 예: 파쿠르 액션과 크래프팅 시스템이 결합된 3D 액션 어드벤처 게임입니다.]

📖 프로젝트 개요

이 프로젝트는 언리얼 엔진(Unreal Engine 5.5)과 C++를 기반으로 개발된 액션 RPG/어드벤처 게임입니다. 체계적인 스탯 관리 시스템, 드래그 앤 드롭을 지원하는 인벤토리 및 크래프팅, 그리고 비헤이비어 트리(Behavior Tree)를 활용한 정교한 적 AI를 특징으로 합니다.

✨ 주요 기능 (Key Features)

🗡️ 전투 및 파쿠르 시스템

스탯 시스템 (StatComponent): 체력, 스태미나 등 캐릭터의 주요 능력치 관리.

특수 공격 (SpecialAttackComponent): 플레이어 및 적 캐릭터의 고유 스킬 및 광역/투사체 공격.

🎒 인벤토리 및 아이템 (Inventory & Items)

드래그 앤 드롭 인벤토리: UI 상에서 직관적으로 아이템을 이동하고 장착(InventoryDragDropOperation).

크래프팅 시스템 (CraftingStation): 수집한 재료를 조합하여 새로운 아이템이나 무기를 제작.

루팅 및 상호작용 (InteractionInterface): 월드 내 아이템 획득 및 문(Door)/NPC 상호작용.

🧠 적 AI 시스템 (Enemy AI)

비헤이비어 트리(Behavior Tree) 기반 설계: 순찰(BTTask_PatrolRandom), 추적(BTTask_ChaseTarget), 공격 상황 판단을 통한 능동적인 AI.

보스 몬스터 (BossEnemyCharacter): 일반 몬스터와 차별화된 패턴(AOE 인디케이터, 유도 투사체 등) 및 특수 공격 구사.

🗺️ 레벨 및 UI

레벨 프로그레션 (LevelDoor, EventZone): 특정 조건 달성 시 다음 구역/레벨로 이동.

직관적인 HUD: 나침반(CompassWidget), 퀘스트 마커(QuestItemWidget), 결과 화면 등 플레이어 편의성을 고려한 UI 구성.

📂 프로젝트 구조 (Directory Structure)

Team3_Project/
├── Core/       # GameMode, GameState, GameInstance 및 레벨/이벤트 관리 매니저
├── Player/     # 플레이어 캐릭터 컨트롤 및 로직
├── Enemy/      # 적/보스 캐릭터, AI Controller, Behavior Tree (Tasks, Decorators 등)
├── Item/       # 무기, 투사체, 인벤토리 컴포넌트, 크래프팅 베이스
├── UI/         # 메인 HUD, 인벤토리, 크래프팅, 시작/일시정지 메뉴 위젯
└── Shared/     # 공통 인터페이스(Interaction, Equipable), 스탯 및 범용 컴포넌트


🚀 시작하기 (Getting Started)

요구 사항 (Prerequisites)

Unreal Engine [5.5] 이상

Visual Studio 2022 (C++ 게임 개발 워크로드 포함)



설치 및 빌드 (Installation)

이 저장소를 클론합니다.

.uproject 파일을 우클릭하고 Generate Visual Studio project files를 선택합니다.

생성된 .sln 파일을 열고 Development Editor 모드로 빌드합니다.

언리얼 에디터에서 프로젝트를 실행합니다.



🎮 조작법 (Controls)

W/A/S/D - 이동

Spacebar - 점프

LMB - 기본 공격

RMB - 줌

F - 상호작용 (아이템 줍기, 문 열기 등)

I 또는 Tab - 인벤토리 열기

👥 팀원 및 역할 (Team Members)

[조수경][TeamLeader] - 핵심 UI 개발 담당, 팀 일정 조율

[정찬호][GitMaster] - Enemy 및 AI, Behavior Tree 개발 담당, Git 담당

[주철민][ProjectManager] - 인벤토리 및 아이템,조합, 관련 UI 개발 담당, 전체적인 프로젝트 상태 조율

[박민재] - 게임 코어 시스템 개발 및 레벨 디자인 조정 담당

[김상민] - 캐릭터 모션매칭 및 무기 애니메이션 등 총괄적인 캐릭터 관련 개발 담당

이 프로젝트는 [내일배움캠프 Unreal 7기]의 팀 프로젝트로 진행되었습니다.
