kevent (  )
if ( listen_fd ) // 새로운 연결 요청 (listen fd는 read event 밖에 없음)
{
    새로 연결된 fd에 대해서 read와 write 이벤트 등록하고 read는 enable, write는 disable 해놓기, timer세팅도 같이(timeout 용)
}
else if ( client_fd & read event ) // req 요청
{
    request 읽기 (kevent struct의 data 변수를 통해 req한 번에 읽기 가능)
        - (-1) 반환될 때 -> 회귀 -> kevent에서 멈출 가능성 있음 
            - EV_SET 으로 EVFILT_USER 를 통해 커스텀 이벤트 지정이 가능하다.  
        - 값이 바로 반환될 때 -> 바로 진행 (문제 없음)
    request 파싱하기

    if ( get method )
    {
        if ( file get )
        {
            file 열기
            그리고 해당 파일 fd를 kque에  read 이벤트로 등록'만'하고 빠져나오기.
            EV_SET(READ);
        }
        else if ( cgi get )
        {
            pipe 열고
            읽기 fd를  kque에 read 이벤트로 등록'만'하고 빠져나오기.
            EV_SET(READ);
        }
        kevent 등록을 할 때 fd 기준이니까, udata -> client metadata * 넣기. 
    }
    else if ( post method )
    {
        파일 만들고 ( open 함수로 만들고 )
        해당 파일 fd를 kque에 write 이벤트로 등록'만'하고 빠져나오기.
    }
    else if ( delete method )
    {
        delete 처리하고,
        response 메시지 다 만들고,
        해당 메서드 요청한 클라이언트 fd에 대해서 write를 enable로 바꿔주기.
    }
}
else if (client_fd & write event) // response 준비완료
{
    msg composing
        - connection 연결 방식 플래그로 확인(keep-alive or close)
    send response.
    클라이언트의 write 이벤트 disable로 변경.
}
else if (file_fd or pipe_fd & read event)
{
    val = read
    - (-1)
    - val(구체적인 값)
    get 메서드에서 요청한 파일 또는 cgi 반환 데이터 읽기(이거 역시 kevent 구조체의 data 변수를 통해 한 번에 읽기 가능할 듯)
    response 메시지 만들기
    해당 메서드 요청한 '클라이언트 fd'에 대해서 'write' 이벤트를 enable로 바꿔주기. -> 회귀 
}
else if (file_fd & write event) # post
{
    if (넣기)
    {
        post 메서드 처리
        만든 파일에 내용 집어넣기(write)
    }
    else if ( 무결성 검사 )
    {
        파일 무결성 검사.
        if (OK)
        {
            response 메시지 만들기
        }
        else
        {
            500 메시지 만들기
        }
        해당 메서드 요청한 클라이언트 fd에 대해서 write 이벤트를 enable로 바꿔주기.
    }
}
else if (client_fd & EV_EOF) # client connection close()
{
    클라이언트 측에서 먼저 연결을 끊음.
    해당 fd에 대해서 이벤트 전부 삭제 해주고
    client meta 데이터에서 삭제.
    fd 닫기.
}
else if (client_fd & TIMER) # timeout
{
    if (under processing) {
    clientmetadata에 변수 <- 타임 아웃 되었음을 플래그 설정함 
    msgcomposer 가 이를 읽고 connection : close 를 붙여줌(no keep-alive)
    }
    else if (processing finish) {
    지속가능한 연결시간 초과
    우아한 연결 끊기
    이벤트 전부 삭제
    client meta 데이터에서 삭제.
    fd 닫기.
    }
}