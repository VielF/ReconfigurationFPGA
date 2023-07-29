#include "masterthread.h"

#include <QtSerialPort/QSerialPort>
#include <QElapsedTimer>
#include <QTime>

#include <stdio.h>
#include <user_defines.h>
#include <AESdef.h>
#include <AES.h>
#define ENCODE 1
#define DECODE 1

const char ENCODE_KEY[]= {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
const char DECODE_KEY[]= {0x13,0x11,0x1D,0x7F,0xE3,0x94,0x4A,0x17,0xF3,0x07,0xA7,0x8B,0x4D,0x2B,0x30,0xC5};


QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}

#include <QDebug>
MasterThread::~MasterThread(){
    qDebug() << "Destruindo thread";
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void MasterThread::transaction(const QString &portName, int waitTimeout, const QString &request){
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->request = request;

    if (!isRunning())
        start();
    else
        cond.wakeOne();
}

void MasterThread::run(){
    FILE *fin;
    int size_file;
    unsigned int qtd_balance;
    int cout_pages = 0, cout_CB_PAGE = 0;
    unsigned char BALANCE_HEADER = 0;
    unsigned char i,j, k = 0, p=0, test_aux;
    unsigned char dado;
    unsigned char packet[22] = {NULL};
    unsigned char myData8[16] = {NULL};
    unsigned char EOT[22] = {NULL};
    unsigned char AESkey[16];
    int currentWaitTimeout = waitTimeout;
    bool currentPortNameChanged = false, balance_nec = false;
    int test_COUNT = 0;

    fin = fopen("output_file_pulse_auto.rpd","rb"); //gray code funcionando
    //fin = fopen("output_filewithout_reset_auto.rpd","rb"); //gray code funcionando
    //fin = fopen("output_file_exp2v4with_compression_auto.rpd","rb");
    //fin = fopen("output_file_exp2v3with_compression_auto.rpd","rb");
    //fin = fopen("output_file_exp2v2_with_compression_auto.rpd","rb");
    //fin = fopen("output_file_exp2_with_compression_auto.rpd","rb");
    //fin = fopen("output_file_without_compression.rpd","rb");
    //fin = fopen("pof_exp_2v3_auto.rpd","rb");
    if((fin)== NULL){
        printf("\nErro: nao abriu o arquivo de Entrada\n");
        return;
    }else{
        k=1;
    }
    fseek (fin, 0, SEEK_END);
    size_file=ftell (fin);
    fseek (fin, 0, SEEK_SET);
    int qtd_pages = size_file/256;
    qtd_balance = size_file%256;
    BALANCE_HEADER = (unsigned char)qtd_balance;
    mutex.lock();
    QElapsedTimer time_reconfiguration;
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    QString currentRequest = request;
    mutex.unlock();

    QSerialPort serial;
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);

    while (!quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }

        // write request
        QByteArray requestData;
        j=0;

        requestData[0]=INIT_TRANSM; //Inicia a trasmissão
        time_reconfiguration.start();
        serial.write(requestData);
        if (serial.waitForReadyRead(-1)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(10))
                responseData += serial.readAll();

            QString response(responseData);
            emit this->response(response);

        }
        //time_reconfiguration.start();
        while (fread(&dado,sizeof(unsigned char),1,fin) == 1){
            myData8[j] = dado;
            j++;
            if(fin->_cnt == 0){
                if((j < 16)){
                    int completar = 15 - j;
                    for(int p = 15-completar; p<16;p++){
                        myData8[p] = 0xFF;
                    }
                }
                if((cout_pages == qtd_pages) && qtd_balance > 0){
                     balance_nec = true;
                     p++;
                }
                j=16;
            }
            if(j > 15){
                for(i=0;i<16;i++){
                    AESkey[i]=ENCODE_KEY[i];
                }
                AESEncode(myData8,AESkey);
                j=0;
                for(j=0;j<16;j++){
                    packet[j+5] = myData8[j];
                }
                cout_CB_PAGE++;
                requestData = QByteArray((char*)packet,sizeof(packet));
                requestData[0] = SYN; //SYN - sincronismo 50 = 0x32
                requestData[1] = SYN; //SIN - sincronismo 50 = 0x32
                requestData[2] = (balance_nec == false) ? SOH : SOH_BAL;  //SOH - inicio cabeçalho 1 = 0x01 - SOH_BAL = 2 - Balanceamento
                requestData[3] = (balance_nec == false) ? DEFAULT_HEADER : BALANCE_HEADER; //cabeçalho - 0x10 = 16(n pacotes) 0xFF = fim do arquivo
                requestData[4] = STX;  //STX - inicio do payload 2 = 0x02
                requestData[21] = ETX; //ETX - fim payload 3 = 0x03
                if(requestData[3] == BALANCE_HEADER){
                    test_aux = 0;
                }
                serial.write(requestData);

                j=0;

                if(cout_CB_PAGE == 16){
                    cout_pages++;
                    cout_CB_PAGE=0;
                    test_COUNT = 16;
                }


                if (serial.waitForReadyRead(-1)) {
                    QByteArray responseData = serial.readAll();
                    while (serial.waitForReadyRead(10))
                        responseData += serial.readAll();

                    QString response(responseData);  
                    emit this->response(response);

                }
                //if(test_COUNT == 16){
                  //  QString time_rec_s = QString("%1 ms").arg(time_reconfiguration.elapsed());
                  //  emit this->time_rec(time_rec_s);
                //}
            }
            k++;
        }
        fclose(fin);
        requestData = QByteArray((char*)EOT,sizeof(EOT));
        requestData[0] = SYN; //SYN - sincronismo 50 = 0x32
        requestData[1] = SYN; //SIN - sincronismo 50 = 0x32
        requestData[2] = SOH_END;  //SOH - inicio cabeçalho fim do arquivo 3 = 0x03
        requestData[3] = DONE_HEADER; //cabeçalho - 0xFF(255) = fim do arquivo
        requestData[4] = STX;  //STX - inicio do payload 2 = 0x02
        requestData[21] = ETX; //ETX - fim payload 3 = 0x03
        serial.write(requestData);
        if (serial.waitForReadyRead(-1)) {
            QByteArray responseData = serial.readAll();
            while (serial.waitForReadyRead(10))
                responseData += serial.readAll();

            QString response(responseData);
            emit this->response(response);
        }
        time_reconfiguration.elapsed();
        QString time_rec_s = QString("%1 ms").arg(time_reconfiguration.elapsed());
        emit this->time_rec(time_rec_s);
        mutex.lock();
        cond.wait(&mutex);
        if (currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRequest = request;
        mutex.unlock();
        //quit = true;
        //serial.close();
    }
}
