## Эксплуатация (Kubernetes)

Развертывание сервиса и его сопровождение выполняются в локальном кластере MicroK8s.

### 1. Развертывание приложения
Для применения всех манифестов (Namespace, ConfigMap, Deployment, Service) выполните команду:
`microk8s kubectl apply -f k8s/`
Перед этим необходимо создать Secret (он не хранится в репозитории):
`microk8s kubectl -n cryptofile create secret generic cryptofile-secret --from-literal=APP_API_TOKEN=supersecret`

### 2. Проверка состояния
Посмотреть список запущенных подов:
`microk8s kubectl -n cryptofile get pods`
Ожидается: 3 пода в статусе `Running` (READY 1/1).
Посмотреть статус Deployment (доступные реплики):
`microk8s kubectl -n cryptofile get deployment cryptofile`

### 3. Балансировка нагрузки
Чтобы увидеть, как Service распределяет запросы между репликами, запустите временный под с curl:
`microk8s kubectl -n cryptofile run test-curl --rm -it --restart=Never --image=curlimages/curl -- sh -c 'while true; do curl -s http://cryptofile/version; echo; sleep 1; done'`
Ожидается: запросы поочередно приходят на разные поды (видно по имени хоста в ответе).

### 4. Самовосстановление (Self-healing)
Удалите один из подов, чтобы проверить, что Deployment создаст новый:
`microk8s kubectl -n cryptofile delete pod <ИМЯ_ПОДА>`
Ожидается: удаленный под исчезнет, а на его месте через несколько секунд появится новый в статусе `Running`.

### 5. Масштабирование
Увеличение количества реплик:
`microk8s kubectl -n cryptofile scale deployment cryptofile --replicas=3`
Ожидается: создание новых подов и увеличение их общего количества до 3.
*(Важно: для декларативного масштабирования нужно изменить параметр `replicas` в `k8s/deployment.yaml` и применить файл).*

### 6. Обновление без простоя (Rolling Update)
Измените тег образа в `k8s/deployment.yaml` (например, с 1.0.0 на 1.0.1) и примените изменения:
`microk8s kubectl apply -f k8s/deployment.yaml`
Следить за ходом обновления:
`microk8s kubectl -n cryptofile rollout status deployment cryptofile`
Ожидается: старые поды постепенно заменяются новыми, сервис не перестает отвечать на запросы (благодаря `maxUnavailable: 0`).

### 7. Откат (Rollback)
Вернуть предыдущую версию приложения:
`microk8s kubectl -n cryptofile rollout undo deployment cryptofile`
Посмотреть историю ревизий:
`microk8s kubectl -n cryptofile rollout history deployment cryptofile`

### 8. Диагностика
Просмотр логов пода:
`microk8s kubectl -n cryptofile logs <ИМЯ_ПОДА>`
Просмотр событий пода (например, при ошибке скачивания образа):
`microk8s kubectl -n cryptofile describe pod <ИМЯ_ПОДА>`

### 9. Доступ к сервису с хоста
Для проверки работы сервиса из браузера или через curl на вашей машине, используйте port-forward:
`microk8s kubectl -n cryptofile port-forward svc/cryptofile 8080:80`
Сервис будет доступен по адресу `http://localhost:8080`.

