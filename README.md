# 주요 기능

- **절차적 던전 생성 시스템 구현**: 플레이할 때마다 새로운 구조의 던전을 생성하여 반복 플레이의 재미를 더합니다.
- **오브젝트 풀링을 통한 최적화 구현**: 오브젝트 풀링(Object Pooling)을 적용하여 메모리 사용을 최적화하고 게임 성능을 향상시켰습니다.
- **머신 러닝을 사용한 보스 패턴 구현**: 비헤이비어 트리(Behavior Tree)를 기반으로 한 적 AI는 플레이어의 행동에 반응하며 다양한 공격 패턴을 구사합니다.

## 1. 절차적 던전 생성 시스템 구현

- **1.1. 그리드 기반 맵 생성**: `ACGeneratedStage` 클래스를 중심으로 그리드 좌표계를 사용하여 방과 복도를 배치하고, 절차적으로 던전의 기본 구조를 생성합니다.

![image](https://github.com/user-attachments/assets/61eecb1e-3b32-481b-b9da-38bc7a6fb5e7)

그리드 내에 (4*4) 크기의 계단을 생성합니다.

입구의 경우 위층의 좌표에 따라 고정되어 생성되며, 출구의 경우 좌표 내 랜덤하게 생성되도록 하였습니다.

![image](https://github.com/user-attachments/assets/ca3143f0-4e0c-4eef-963f-0ae5fe032e52)

나머지 그리드에 무작위로 노드를 생성하도록 하였습니다.

방의 개수, 띄워져 있어야 할 최소한의 거리 등의 파라미터를 통해 조정할 수 있도록 하였습니다.

![image](https://github.com/user-attachments/assets/a2ea7ccf-6807-4a4e-9995-27c8acbb5a0f)

무작위로 보스 스테이지를 선정하여 (5*5) 크기의 그리드로 확장하였습니다.

- **1.2. 방과 복도 연결 알고리즘**: 각기 독립적으로 생성된 방들을 연결하기 위해 Union-Find 알고리즘으로 그룹화하고, A* 알고리즘을 응용하여 최단 경로의 복도를 생성합니다.

![image](https://github.com/user-attachments/assets/1f0c0f12-2b03-46d5-a33d-5bdc7abf1557)

BFS를 통해 가장 가까운 노드 또는 길에 연결하도록 하였습니다.

연결이 완료될 때마다 Union Find를 사용하여 루트 노드를 업데이트하였고,

서로 다른 루트를 가지는 노드 또는 길에 연결하도록 하여 모든 방이 연결되도록 보장하였습니다.

![image](https://github.com/user-attachments/assets/c8d24dca-b88d-430d-8bef-634173ce8d9b)

길 끝에 위치해있고 주변이 정돈되어 있는 노드를 (3*3) 크기의 그리드로 확장하였습니다.

## 2. 오브젝트 풀링을 통한 최적화 구현

HierarchicalInstancedStaticMeshComponent를 사용하여 던전 내부를 이루는 벽, 오브젝트 등을 구현하였으나,

내비게이션 시스템 사용 등에 있어 어려운 점이 있어 StaticMeshComponent를 사용하기로 결정하였습니다.

절차적 생성을 통해 만들어진 그리드에 벽, 바닥, 천장 등의 StaticMeshComponent를 소환하여 스테이지 액터에 Attach하여 구현하였습니다.

플레이어의 현재 그리드 상 좌표를 지속적으로 업데이트하고, Dykstra 알고리즘을 사용하여 모든 그리드의 거리를 업데이트하였습니다.

각 그리드의 플레이어와의 거리가 일정 수치 이상일 경우, StaticMeshComponent를 ObjectPooling 방식을 적용하여

Queue에서 미리 생성되어 있는 StaticMeshComponent를 Pop하거나 다시 Return하는 방식으로 메모리를 최적화하였습니다.



## 3. 머신 러닝을 사용한 보스 패턴 구현

그리드 내에서 플레이어의 움직임 및 행동 패턴을 기록하여 시계열 데이터로 저장 후, 머신러닝을 활용하여 개개인의 플레이 스타일 적응하는 보스의 공격 패턴을 만들고자 하였습니다.

gpt-oss-20b 모델을 고려하였으나, 런타임에서 사용하기에는 무겁다고 판단하여 XGBoost 모델을 사용하였습니다.

기록된 데이터를 학습 데이터로 사용하여 학습하였고, ONNX로 변환 후, 임포트하여 Unreal Engine에 머신러닝을 구현하였습니다.


## 3-1. 1차 시도

Feature : 35가지

- 플레이어가 입력한 버튼
- 플레이어의 현재 Grid 상 위치 (플레이어가 입장한 방의 앞뒤좌우 벽으로부터의 거리)
- 상대 중인 적 캐릭터와의 거리
- 플레이어의 HP 및 스테미나 수치
- 위 Feature들의 n틱 이전 값 (1~5)
- 입력한 버튼의 표준편차 및 이동평균

Output : 입력 될 가능성이 높은 버튼

----------

### 결과

버튼을 Integer에 매핑 후, 카테고리로 사용하여 학습하여 테스트하였습니다.

<img width="386" height="265" alt="image" src="https://github.com/user-attachments/assets/47950ff9-5a7f-4797-9e7c-b6be22106f36" />

플레이어가 입력할 버튼을 예측하는 XGBClassifier 모델입니다.

----------

### 개선점

Importance를 참고하면 주변 환경 변수보다 플레이어가 이전에 입력한 버튼(PlayerButtonSeries)에 과도하게 의존하는 것을 확인하였으며,
  플레이어의 입력을 예측하는 것은 고려해야 할 변수가 많다고 생각하여 폐기하였습니다.

## 3-2. 2차 시도

Feature : 100가지 10 * 10 틱
- 플레이어의 현재 Grid 상 위치 (플레이어가 입장한 방의 앞뒤좌우 벽으로부터의 거리)
- 플레이어의 HP 및 스테미나 수치
- 상대 중인 적 캐릭터와의 거리
- 플레이어의 Forward Vector
- 플레이어가 움직이는 방향(벡터)
- 플레이어가 움직이는 속도

방향벡터의 경우 라디안으로 변환하여 사용

Output : 플레이어가 이동할 각도

----------

### 오류

-- 모델 UniquePtr 호출하는 과정에서 Access Violation 오류 발생

--- GC 오류였음, CNeuralNetwork 객체를 자꾸 지워버림 = AddToRoot 함수 호출하여 해결

----------

### 결과

<img width="363" height="221" alt="image" src="https://github.com/user-attachments/assets/20968070-6edf-4395-a3dc-0a1466c4a89d" />

10.13 MSE: 0.135922

XGBRegressor 모델로 플레이어가 움직일 방향을 예측하는 모델입니다.

모델 Validation 과정 중 정확도가 다소 떨어지는 것을 확인하였으나 데이터의 수량이 부족한 것이라 판단하였습니다.


이전 모델에 비해 주변 환경 요소를 비등하게 고려하고 있음을 확인할 수 있었습니다.

또한, PlayerRelativeDirectionRadian의 비중이 높은 것 또한 플레이어의 움직임 방향을 예측하는 모델인만큼 바람직하다 판단하였습니다.

<img width="461" height="243" alt="image" src="https://github.com/user-attachments/assets/991790ad-71e2-4bbb-ac5b-51b95fb9a460" />

또한 최근 값의 경우 대체로 더 높은 Importance를 갖는 경향을 확인하였습니다.

----------

### 개선점

하지만 실제 플레이 중 정면으로 예측하는 경우가 잦았습니다.

Feature를 추가하기 보다 공격을 시작하는 시점(모델이 예측을 시작할 시점)에서의 데이터만을 수집하여 학습하는 것으로 개선할 예정입니다.
