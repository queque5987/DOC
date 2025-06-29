언리얼 5.2 게임 개발

# 1. BFS, Union Find를 활용한 스테이지 자동 생성
![image](https://github.com/user-attachments/assets/61eecb1e-3b32-481b-b9da-38bc7a6fb5e7)

그리드 내에 (4*4) 크기의 계단을 생성합니다.

입구의 경우 위층의 좌표에 따라 고정되어 생성되며, 출구의 경우 좌표 내 랜덤하게 생성되도록 하였습니다.

![image](https://github.com/user-attachments/assets/ca3143f0-4e0c-4eef-963f-0ae5fe032e52)

나머지 그리드에 무작위로 노드를 생성하도록 하였습니다.

방의 개수, 띄워져 있어야 할 최소한의 거리 등의 파라미터를 통해 조정할 수 있도록 하였습니다.

![image](https://github.com/user-attachments/assets/a2ea7ccf-6807-4a4e-9995-27c8acbb5a0f)

무작위로 보스 스테이지를 선정하여 (5*5) 크기의 그리드로 확장하였습니다.

![image](https://github.com/user-attachments/assets/1f0c0f12-2b03-46d5-a33d-5bdc7abf1557)

BFS를 통해 가장 가까운 노드 또는 길에 연결하도록 하였습니다.

연결이 완료될 때마다 Union Find를 사용하여 루트 노드를 업데이트하였고,

서로 다른 루트를 가지는 노드 또는 길에 연결하도록 하여 모든 방이 연결되도록 보장하였습니다.

![image](https://github.com/user-attachments/assets/c8d24dca-b88d-430d-8bef-634173ce8d9b)

길 끝에 위치해있고 주변이 정돈되어 있는 노드를 (3*3) 크기의 그리드로 확장하였습니다.

# 2. 그리드 기반의 Object Pooling을 활용한 런타임 메모리 최적화

HierarchicalInstancedStaticMeshComponent를 사용하여 던전 내부를 이루는 벽, 오브젝트 등을 구현하였으나,

내비게이션 시스템 사용 등에 있어 어려운 점이 있어 StaticMeshComponent를 사용하기로 결정하였습니다.

각 그리드에 벽, 바닥, 천장 등의 StaticMeshComponent를 소환할 때, ObjectPooling 방식을 적용하여

이미 생성된 컴포넌트의 Transform과 Visibility 등의 파라미터를 변형하여 활용하였습니다.
