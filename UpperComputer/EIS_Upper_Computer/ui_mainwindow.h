/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *RX_Parse;
    QLabel *label_14;
    QLabel *label_12;
    QPlainTextEdit *rx_parse;
    QLabel *label_11;
    QLabel *label_13;
    QLabel *label_10;
    QLabel *label_9;
    QLabel *label_15;
    QLabel *label_16;
    QPushButton *pushButton_parseclear;
    QGroupBox *groupBox_5;
    QPushButton *pushButton_rxclear;
    QGroupBox *groupBox;
    QRadioButton *rxform_ASCII;
    QRadioButton *rxform_HEX;
    QPlainTextEdit *rx_text;
    QGroupBox *groupBox_6;
    QPushButton *pushButton_close;
    QLabel *label_3;
    QLabel *label_5;
    QPushButton *pushButton_open;
    QPushButton *pushButton_refresh;
    QComboBox *box_baudrate;
    QComboBox *box_seriallist;
    QComboBox *box_databit;
    QComboBox *box_parity;
    QComboBox *box_stopbit;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label;
    QTabWidget *tabWidget;
    QWidget *EIS;
    QGroupBox *groupBox_4;
    QLineEdit *Mac_Input;
    QLabel *label_6;
    QGroupBox *groupBox_Light;
    QPushButton *pushButton_LightChg;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QRadioButton *Light_ALL;
    QRadioButton *Light_RO;
    QRadioButton *Light_LO;
    QRadioButton *Light_OFF;
    QLabel *label_7;
    QSpinBox *period;
    QGroupBox *test_result;
    QLineEdit *Mac_Recv_EIS;
    QLabel *label_8;
    QPushButton *pushButton_datasave;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox_Temp;
    QCheckBox *checkBox_Light;
    QCheckBox *checkBox_PM;
    QCheckBox *checkBox_cmd;
    QCheckBox *checkBox_CO2;
    QWidget *GAS;
    QGroupBox *test_result_2;
    QLineEdit *Mac_Recv_GAS;
    QLabel *label_17;
    QPushButton *pushButton_datasave_GAS;
    QCheckBox *checkBox_SO2_GAS;
    QCheckBox *checkBox_H2S_GAS;
    QCheckBox *checkBox_CO_GAS;
    QCheckBox *checkBox_CO2_GAS;
    QCheckBox *checkBox_PM_GAS;
    QCheckBox *checkBox_Result_GAS;
    QCheckBox *checkBox_Temp_Humi_GAS;
    QCheckBox *checkBox_CH4_GAS;
    QCheckBox *checkBox_NH3_GAS;
    QCheckBox *checkBox_NO2_GAS;
    QCheckBox *checkBox_Motor_GAS;
    QGroupBox *groupBox_9;
    QLineEdit *Mac_Input_GAS;
    QLabel *label_18;
    QLabel *label_19;
    QSpinBox *period_2;
    QComboBox *box_sensor_u4;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_22;
    QPushButton *pushButton_typeset_Gas;
    QComboBox *box_sensor_u5;
    QSpinBox *rotation_turns;
    QLabel *label_23;
    QPushButton *pushButton_motor;
    QPushButton *pushButton_periodset_Gas;
    QComboBox *box_sensor_u6;
    QGroupBox *groupBox_3;
    QPushButton *pushButton_send;
    QPushButton *pushButton_txclear;
    QTextEdit *tx_text;
    QGroupBox *groupBox_2;
    QRadioButton *txform_ASCII;
    QRadioButton *txform_HEX;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1560, 1002);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        RX_Parse = new QGroupBox(centralwidget);
        RX_Parse->setObjectName("RX_Parse");
        RX_Parse->setGeometry(QRect(240, 330, 1201, 261));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RX_Parse->sizePolicy().hasHeightForWidth());
        RX_Parse->setSizePolicy(sizePolicy);
        label_14 = new QLabel(RX_Parse);
        label_14->setObjectName("label_14");
        label_14->setGeometry(QRect(500, 30, 53, 15));
        label_14->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_12 = new QLabel(RX_Parse);
        label_12->setObjectName("label_12");
        label_12->setGeometry(QRect(240, 30, 31, 16));
        label_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        rx_parse = new QPlainTextEdit(RX_Parse);
        rx_parse->setObjectName("rx_parse");
        rx_parse->setGeometry(QRect(10, 50, 1071, 191));
        rx_parse->setLineWrapMode(QPlainTextEdit::NoWrap);
        rx_parse->setReadOnly(true);
        label_11 = new QLabel(RX_Parse);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(170, 30, 53, 15));
        label_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_13 = new QLabel(RX_Parse);
        label_13->setObjectName("label_13");
        label_13->setGeometry(QRect(280, 30, 41, 16));
        label_13->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_10 = new QLabel(RX_Parse);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(100, 30, 31, 16));
        label_10->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_9 = new QLabel(RX_Parse);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(30, 30, 53, 15));
        label_9->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_15 = new QLabel(RX_Parse);
        label_15->setObjectName("label_15");
        label_15->setGeometry(QRect(320, 30, 41, 16));
        label_15->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_16 = new QLabel(RX_Parse);
        label_16->setObjectName("label_16");
        label_16->setGeometry(QRect(860, 30, 91, 16));
        label_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        pushButton_parseclear = new QPushButton(RX_Parse);
        pushButton_parseclear->setObjectName("pushButton_parseclear");
        pushButton_parseclear->setGeometry(QRect(1100, 220, 75, 23));
        groupBox_5 = new QGroupBox(centralwidget);
        groupBox_5->setObjectName("groupBox_5");
        groupBox_5->setGeometry(QRect(240, 40, 1201, 271));
        sizePolicy.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy);
        pushButton_rxclear = new QPushButton(groupBox_5);
        pushButton_rxclear->setObjectName("pushButton_rxclear");
        pushButton_rxclear->setGeometry(QRect(1100, 230, 75, 23));
        groupBox = new QGroupBox(groupBox_5);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(1090, 30, 91, 71));
        rxform_ASCII = new QRadioButton(groupBox);
        rxform_ASCII->setObjectName("rxform_ASCII");
        rxform_ASCII->setGeometry(QRect(10, 20, 95, 19));
        rxform_ASCII->setChecked(false);
        rxform_HEX = new QRadioButton(groupBox);
        rxform_HEX->setObjectName("rxform_HEX");
        rxform_HEX->setGeometry(QRect(10, 50, 95, 19));
        rxform_HEX->setChecked(true);
        rx_text = new QPlainTextEdit(groupBox_5);
        rx_text->setObjectName("rx_text");
        rx_text->setGeometry(QRect(10, 30, 1071, 221));
        rx_text->setLineWrapMode(QPlainTextEdit::NoWrap);
        rx_text->setReadOnly(true);
        groupBox_6 = new QGroupBox(centralwidget);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setGeometry(QRect(20, 60, 211, 431));
        sizePolicy.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy);
        pushButton_close = new QPushButton(groupBox_6);
        pushButton_close->setObjectName("pushButton_close");
        pushButton_close->setGeometry(QRect(120, 370, 75, 23));
        label_3 = new QLabel(groupBox_6);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 110, 53, 15));
        label_5 = new QLabel(groupBox_6);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(30, 190, 53, 15));
        pushButton_open = new QPushButton(groupBox_6);
        pushButton_open->setObjectName("pushButton_open");
        pushButton_open->setGeometry(QRect(30, 370, 75, 23));
        pushButton_refresh = new QPushButton(groupBox_6);
        pushButton_refresh->setObjectName("pushButton_refresh");
        pushButton_refresh->setGeometry(QRect(30, 320, 75, 23));
        box_baudrate = new QComboBox(groupBox_6);
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->addItem(QString());
        box_baudrate->setObjectName("box_baudrate");
        box_baudrate->setGeometry(QRect(90, 70, 69, 22));
        box_seriallist = new QComboBox(groupBox_6);
        box_seriallist->setObjectName("box_seriallist");
        box_seriallist->setGeometry(QRect(90, 30, 69, 22));
        box_databit = new QComboBox(groupBox_6);
        box_databit->addItem(QString());
        box_databit->addItem(QString());
        box_databit->addItem(QString());
        box_databit->addItem(QString());
        box_databit->setObjectName("box_databit");
        box_databit->setGeometry(QRect(90, 110, 69, 22));
        box_parity = new QComboBox(groupBox_6);
        box_parity->addItem(QString());
        box_parity->addItem(QString());
        box_parity->addItem(QString());
        box_parity->setObjectName("box_parity");
        box_parity->setGeometry(QRect(90, 190, 69, 22));
        box_stopbit = new QComboBox(groupBox_6);
        box_stopbit->addItem(QString());
        box_stopbit->addItem(QString());
        box_stopbit->addItem(QString());
        box_stopbit->setObjectName("box_stopbit");
        box_stopbit->setGeometry(QRect(90, 150, 69, 22));
        label_4 = new QLabel(groupBox_6);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(30, 150, 53, 15));
        label_2 = new QLabel(groupBox_6);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 70, 53, 15));
        label = new QLabel(groupBox_6);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 53, 15));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(820, 600, 621, 311));
        EIS = new QWidget();
        EIS->setObjectName("EIS");
        groupBox_4 = new QGroupBox(EIS);
        groupBox_4->setObjectName("groupBox_4");
        groupBox_4->setGeometry(QRect(30, 20, 331, 251));
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        Mac_Input = new QLineEdit(groupBox_4);
        Mac_Input->setObjectName("Mac_Input");
        Mac_Input->setGeometry(QRect(20, 50, 251, 31));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 30, 53, 15));
        groupBox_Light = new QGroupBox(groupBox_4);
        groupBox_Light->setObjectName("groupBox_Light");
        groupBox_Light->setGeometry(QRect(10, 140, 321, 101));
        groupBox_Light->setCheckable(false);
        pushButton_LightChg = new QPushButton(groupBox_Light);
        pushButton_LightChg->setObjectName("pushButton_LightChg");
        pushButton_LightChg->setGeometry(QRect(240, 70, 75, 23));
        layoutWidget = new QWidget(groupBox_Light);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 40, 304, 21));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        Light_ALL = new QRadioButton(layoutWidget);
        Light_ALL->setObjectName("Light_ALL");
        Light_ALL->setChecked(true);

        horizontalLayout->addWidget(Light_ALL);

        Light_RO = new QRadioButton(layoutWidget);
        Light_RO->setObjectName("Light_RO");

        horizontalLayout->addWidget(Light_RO);

        Light_LO = new QRadioButton(layoutWidget);
        Light_LO->setObjectName("Light_LO");

        horizontalLayout->addWidget(Light_LO);

        Light_OFF = new QRadioButton(layoutWidget);
        Light_OFF->setObjectName("Light_OFF");

        horizontalLayout->addWidget(Light_OFF);

        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(20, 100, 53, 15));
        period = new QSpinBox(groupBox_4);
        period->setObjectName("period");
        period->setGeometry(QRect(80, 100, 61, 22));
        period->setWrapping(false);
        period->setMaximum(3600);
        period->setValue(60);
        test_result = new QGroupBox(EIS);
        test_result->setObjectName("test_result");
        test_result->setGeometry(QRect(390, 20, 211, 251));
        sizePolicy.setHeightForWidth(test_result->sizePolicy().hasHeightForWidth());
        test_result->setSizePolicy(sizePolicy);
        Mac_Recv_EIS = new QLineEdit(test_result);
        Mac_Recv_EIS->setObjectName("Mac_Recv_EIS");
        Mac_Recv_EIS->setGeometry(QRect(10, 40, 171, 21));
        label_8 = new QLabel(test_result);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(10, 20, 53, 15));
        pushButton_datasave = new QPushButton(test_result);
        pushButton_datasave->setObjectName("pushButton_datasave");
        pushButton_datasave->setGeometry(QRect(120, 220, 75, 23));
        layoutWidget1 = new QWidget(test_result);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(10, 80, 73, 121));
        verticalLayout_2 = new QVBoxLayout(layoutWidget1);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        checkBox_Temp = new QCheckBox(layoutWidget1);
        checkBox_Temp->setObjectName("checkBox_Temp");

        verticalLayout_2->addWidget(checkBox_Temp);

        checkBox_Light = new QCheckBox(layoutWidget1);
        checkBox_Light->setObjectName("checkBox_Light");

        verticalLayout_2->addWidget(checkBox_Light);

        checkBox_PM = new QCheckBox(layoutWidget1);
        checkBox_PM->setObjectName("checkBox_PM");

        verticalLayout_2->addWidget(checkBox_PM);

        checkBox_cmd = new QCheckBox(layoutWidget1);
        checkBox_cmd->setObjectName("checkBox_cmd");

        verticalLayout_2->addWidget(checkBox_cmd);

        checkBox_CO2 = new QCheckBox(layoutWidget1);
        checkBox_CO2->setObjectName("checkBox_CO2");

        verticalLayout_2->addWidget(checkBox_CO2);

        tabWidget->addTab(EIS, QString());
        GAS = new QWidget();
        GAS->setObjectName("GAS");
        test_result_2 = new QGroupBox(GAS);
        test_result_2->setObjectName("test_result_2");
        test_result_2->setGeometry(QRect(390, 20, 211, 251));
        sizePolicy.setHeightForWidth(test_result_2->sizePolicy().hasHeightForWidth());
        test_result_2->setSizePolicy(sizePolicy);
        Mac_Recv_GAS = new QLineEdit(test_result_2);
        Mac_Recv_GAS->setObjectName("Mac_Recv_GAS");
        Mac_Recv_GAS->setGeometry(QRect(10, 40, 171, 21));
        label_17 = new QLabel(test_result_2);
        label_17->setObjectName("label_17");
        label_17->setGeometry(QRect(10, 20, 53, 15));
        pushButton_datasave_GAS = new QPushButton(test_result_2);
        pushButton_datasave_GAS->setObjectName("pushButton_datasave_GAS");
        pushButton_datasave_GAS->setGeometry(QRect(120, 220, 75, 23));
        checkBox_SO2_GAS = new QCheckBox(test_result_2);
        checkBox_SO2_GAS->setObjectName("checkBox_SO2_GAS");
        checkBox_SO2_GAS->setGeometry(QRect(10, 140, 49, 19));
        checkBox_H2S_GAS = new QCheckBox(test_result_2);
        checkBox_H2S_GAS->setObjectName("checkBox_H2S_GAS");
        checkBox_H2S_GAS->setGeometry(QRect(10, 110, 49, 19));
        checkBox_CO_GAS = new QCheckBox(test_result_2);
        checkBox_CO_GAS->setObjectName("checkBox_CO_GAS");
        checkBox_CO_GAS->setGeometry(QRect(70, 110, 49, 19));
        checkBox_CO2_GAS = new QCheckBox(test_result_2);
        checkBox_CO2_GAS->setObjectName("checkBox_CO2_GAS");
        checkBox_CO2_GAS->setGeometry(QRect(120, 140, 49, 19));
        checkBox_PM_GAS = new QCheckBox(test_result_2);
        checkBox_PM_GAS->setObjectName("checkBox_PM_GAS");
        checkBox_PM_GAS->setGeometry(QRect(10, 170, 49, 19));
        checkBox_Result_GAS = new QCheckBox(test_result_2);
        checkBox_Result_GAS->setObjectName("checkBox_Result_GAS");
        checkBox_Result_GAS->setGeometry(QRect(70, 200, 81, 19));
        checkBox_Temp_Humi_GAS = new QCheckBox(test_result_2);
        checkBox_Temp_Humi_GAS->setObjectName("checkBox_Temp_Humi_GAS");
        checkBox_Temp_Humi_GAS->setGeometry(QRect(70, 170, 59, 19));
        checkBox_CH4_GAS = new QCheckBox(test_result_2);
        checkBox_CH4_GAS->setObjectName("checkBox_CH4_GAS");
        checkBox_CH4_GAS->setGeometry(QRect(10, 80, 47, 19));
        checkBox_NH3_GAS = new QCheckBox(test_result_2);
        checkBox_NH3_GAS->setObjectName("checkBox_NH3_GAS");
        checkBox_NH3_GAS->setGeometry(QRect(70, 80, 49, 19));
        checkBox_NO2_GAS = new QCheckBox(test_result_2);
        checkBox_NO2_GAS->setObjectName("checkBox_NO2_GAS");
        checkBox_NO2_GAS->setGeometry(QRect(70, 140, 48, 19));
        checkBox_Motor_GAS = new QCheckBox(test_result_2);
        checkBox_Motor_GAS->setObjectName("checkBox_Motor_GAS");
        checkBox_Motor_GAS->setGeometry(QRect(10, 200, 59, 19));
        groupBox_9 = new QGroupBox(GAS);
        groupBox_9->setObjectName("groupBox_9");
        groupBox_9->setGeometry(QRect(30, 20, 331, 251));
        sizePolicy.setHeightForWidth(groupBox_9->sizePolicy().hasHeightForWidth());
        groupBox_9->setSizePolicy(sizePolicy);
        Mac_Input_GAS = new QLineEdit(groupBox_9);
        Mac_Input_GAS->setObjectName("Mac_Input_GAS");
        Mac_Input_GAS->setGeometry(QRect(20, 50, 251, 31));
        label_18 = new QLabel(groupBox_9);
        label_18->setObjectName("label_18");
        label_18->setGeometry(QRect(20, 30, 53, 15));
        label_19 = new QLabel(groupBox_9);
        label_19->setObjectName("label_19");
        label_19->setGeometry(QRect(20, 90, 53, 15));
        period_2 = new QSpinBox(groupBox_9);
        period_2->setObjectName("period_2");
        period_2->setGeometry(QRect(80, 90, 61, 22));
        period_2->setWrapping(false);
        period_2->setMaximum(3600);
        period_2->setValue(60);
        box_sensor_u4 = new QComboBox(groupBox_9);
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString());
        box_sensor_u4->addItem(QString::fromUtf8("Temp"));
        box_sensor_u4->addItem(QString::fromUtf8("Humi"));
        box_sensor_u4->addItem(QString());
        box_sensor_u4->setObjectName("box_sensor_u4");
        box_sensor_u4->setGeometry(QRect(20, 180, 69, 22));
        label_20 = new QLabel(groupBox_9);
        label_20->setObjectName("label_20");
        label_20->setGeometry(QRect(30, 160, 53, 15));
        label_21 = new QLabel(groupBox_9);
        label_21->setObjectName("label_21");
        label_21->setGeometry(QRect(140, 160, 53, 15));
        label_22 = new QLabel(groupBox_9);
        label_22->setObjectName("label_22");
        label_22->setGeometry(QRect(250, 160, 53, 15));
        pushButton_typeset_Gas = new QPushButton(groupBox_9);
        pushButton_typeset_Gas->setObjectName("pushButton_typeset_Gas");
        pushButton_typeset_Gas->setGeometry(QRect(240, 220, 71, 23));
        box_sensor_u5 = new QComboBox(groupBox_9);
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString());
        box_sensor_u5->addItem(QString::fromUtf8("Temp"));
        box_sensor_u5->addItem(QString::fromUtf8("Humi"));
        box_sensor_u5->addItem(QString());
        box_sensor_u5->setObjectName("box_sensor_u5");
        box_sensor_u5->setGeometry(QRect(130, 180, 69, 22));
        rotation_turns = new QSpinBox(groupBox_9);
        rotation_turns->setObjectName("rotation_turns");
        rotation_turns->setGeometry(QRect(80, 120, 61, 22));
        rotation_turns->setWrapping(false);
        rotation_turns->setMaximum(100);
        rotation_turns->setValue(1);
        label_23 = new QLabel(groupBox_9);
        label_23->setObjectName("label_23");
        label_23->setGeometry(QRect(20, 120, 53, 15));
        pushButton_motor = new QPushButton(groupBox_9);
        pushButton_motor->setObjectName("pushButton_motor");
        pushButton_motor->setGeometry(QRect(240, 120, 71, 23));
        pushButton_periodset_Gas = new QPushButton(groupBox_9);
        pushButton_periodset_Gas->setObjectName("pushButton_periodset_Gas");
        pushButton_periodset_Gas->setGeometry(QRect(240, 90, 71, 23));
        box_sensor_u6 = new QComboBox(groupBox_9);
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString());
        box_sensor_u6->addItem(QString::fromUtf8("Temp"));
        box_sensor_u6->addItem(QString::fromUtf8("Humi"));
        box_sensor_u6->addItem(QString());
        box_sensor_u6->setObjectName("box_sensor_u6");
        box_sensor_u6->setGeometry(QRect(240, 180, 69, 22));
        tabWidget->addTab(GAS, QString());
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(250, 600, 531, 311));
        sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy);
        pushButton_send = new QPushButton(groupBox_3);
        pushButton_send->setObjectName("pushButton_send");
        pushButton_send->setGeometry(QRect(390, 150, 75, 23));
        pushButton_txclear = new QPushButton(groupBox_3);
        pushButton_txclear->setObjectName("pushButton_txclear");
        pushButton_txclear->setGeometry(QRect(390, 120, 75, 23));
        tx_text = new QTextEdit(groupBox_3);
        tx_text->setObjectName("tx_text");
        tx_text->setGeometry(QRect(10, 30, 361, 141));
        groupBox_2 = new QGroupBox(groupBox_3);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(390, 30, 91, 71));
        txform_ASCII = new QRadioButton(groupBox_2);
        txform_ASCII->setObjectName("txform_ASCII");
        txform_ASCII->setGeometry(QRect(10, 20, 95, 19));
        txform_ASCII->setChecked(false);
        txform_HEX = new QRadioButton(groupBox_2);
        txform_HEX->setObjectName("txform_HEX");
        txform_HEX->setGeometry(QRect(10, 50, 95, 19));
        txform_HEX->setChecked(true);
        MainWindow->setCentralWidget(centralwidget);
        tabWidget->raise();
        groupBox_6->raise();
        groupBox_5->raise();
        RX_Parse->raise();
        groupBox_3->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1560, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        box_baudrate->setCurrentIndex(3);
        box_databit->setCurrentIndex(3);
        tabWidget->setCurrentIndex(1);
        box_sensor_u4->setCurrentIndex(0);
        box_sensor_u5->setCurrentIndex(0);
        box_sensor_u6->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\346\231\272\350\203\275\347\226\217\346\225\243\346\240\207/\346\260\224\344\275\223\346\243\200\346\265\213\344\273\252\346\265\213\350\257\225\344\270\212\344\275\215\346\234\272", nullptr));
        RX_Parse->setTitle(QCoreApplication::translate("MainWindow", "RX Parse", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Payload", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", " \347\261\273\345\236\213", nullptr));
        rx_parse->setPlainText(QString());
        label_11->setText(QCoreApplication::translate("MainWindow", "MAC", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\345\212\237\350\203\275\347\240\201", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "SN", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\351\241\271\347\233\256", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "length", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Payload_Parse", nullptr));
        pushButton_parseclear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\350\247\243\346\236\220", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "RX Data", nullptr));
        pushButton_rxclear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\346\216\245\346\224\266", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\346\216\245\346\224\266\346\225\260\346\215\256\346\240\274\345\274\217", nullptr));
        rxform_ASCII->setText(QCoreApplication::translate("MainWindow", "ASCII", nullptr));
        rxform_HEX->setText(QCoreApplication::translate("MainWindow", "HEX", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        pushButton_close->setText(QCoreApplication::translate("MainWindow", "\345\205\263\351\227\255\344\270\262\345\217\243", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\345\245\207\345\201\266\346\240\241\351\252\214", nullptr));
        pushButton_open->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        pushButton_refresh->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260\344\270\262\345\217\243", nullptr));
        box_baudrate->setItemText(0, QCoreApplication::translate("MainWindow", "1200", nullptr));
        box_baudrate->setItemText(1, QCoreApplication::translate("MainWindow", "2400", nullptr));
        box_baudrate->setItemText(2, QCoreApplication::translate("MainWindow", "4800", nullptr));
        box_baudrate->setItemText(3, QCoreApplication::translate("MainWindow", "9600", nullptr));
        box_baudrate->setItemText(4, QCoreApplication::translate("MainWindow", "19200", nullptr));
        box_baudrate->setItemText(5, QCoreApplication::translate("MainWindow", "38400", nullptr));
        box_baudrate->setItemText(6, QCoreApplication::translate("MainWindow", "57600", nullptr));
        box_baudrate->setItemText(7, QCoreApplication::translate("MainWindow", "115200", nullptr));

        box_databit->setItemText(0, QCoreApplication::translate("MainWindow", "5", nullptr));
        box_databit->setItemText(1, QCoreApplication::translate("MainWindow", "6", nullptr));
        box_databit->setItemText(2, QCoreApplication::translate("MainWindow", "7", nullptr));
        box_databit->setItemText(3, QCoreApplication::translate("MainWindow", "8", nullptr));

        box_parity->setItemText(0, QCoreApplication::translate("MainWindow", "None", nullptr));
        box_parity->setItemText(1, QCoreApplication::translate("MainWindow", "Even", nullptr));
        box_parity->setItemText(2, QCoreApplication::translate("MainWindow", "Odd", nullptr));

        box_stopbit->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        box_stopbit->setItemText(1, QCoreApplication::translate("MainWindow", "1.5", nullptr));
        box_stopbit->setItemText(2, QCoreApplication::translate("MainWindow", "2", nullptr));

        label_4->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\351\200\211\346\213\251", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\346\216\247\345\210\266", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "MAC", nullptr));
        groupBox_Light->setTitle(QCoreApplication::translate("MainWindow", "\347\201\257\345\205\211\346\216\247\345\210\266", nullptr));
        pushButton_LightChg->setText(QCoreApplication::translate("MainWindow", "\347\241\256\350\256\244\345\210\207\346\215\242", nullptr));
        Light_ALL->setText(QCoreApplication::translate("MainWindow", "\345\217\214\344\276\247\347\202\271\344\272\256", nullptr));
        Light_RO->setText(QCoreApplication::translate("MainWindow", "\344\273\205\345\217\263\344\276\247\344\272\256", nullptr));
        Light_LO->setText(QCoreApplication::translate("MainWindow", "\344\273\205\345\267\246\344\276\247\344\272\256", nullptr));
        Light_OFF->setText(QCoreApplication::translate("MainWindow", "\347\201\257\345\205\211\347\206\204\347\201\255", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240\345\221\250\346\234\237", nullptr));
        test_result->setTitle(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\347\273\223\346\236\234", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "MAC", nullptr));
        pushButton_datasave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        checkBox_Temp->setText(QCoreApplication::translate("MainWindow", "\346\270\251\346\271\277\345\272\246", nullptr));
        checkBox_Light->setText(QCoreApplication::translate("MainWindow", "\347\201\257\345\205\211\346\214\207\347\244\272", nullptr));
        checkBox_PM->setText(QCoreApplication::translate("MainWindow", "PM\345\200\274", nullptr));
        checkBox_cmd->setText(QCoreApplication::translate("MainWindow", "\346\214\207\344\273\244\346\216\247\345\210\266", nullptr));
        checkBox_CO2->setText(QCoreApplication::translate("MainWindow", "CO2", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(EIS), QCoreApplication::translate("MainWindow", "\347\226\217\346\225\243\346\240\207", nullptr));
        test_result_2->setTitle(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225\347\273\223\346\236\234", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "MAC", nullptr));
        pushButton_datasave_GAS->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        checkBox_SO2_GAS->setText(QCoreApplication::translate("MainWindow", "SO2", nullptr));
        checkBox_H2S_GAS->setText(QCoreApplication::translate("MainWindow", "H2S", nullptr));
        checkBox_CO_GAS->setText(QCoreApplication::translate("MainWindow", "CO", nullptr));
        checkBox_CO2_GAS->setText(QCoreApplication::translate("MainWindow", "CO2", nullptr));
        checkBox_PM_GAS->setText(QCoreApplication::translate("MainWindow", "PM", nullptr));
        checkBox_Result_GAS->setText(QCoreApplication::translate("MainWindow", "Result OK", nullptr));
        checkBox_Temp_Humi_GAS->setText(QCoreApplication::translate("MainWindow", "\346\270\251\346\271\277\345\272\246", nullptr));
        checkBox_CH4_GAS->setText(QCoreApplication::translate("MainWindow", "CH4", nullptr));
        checkBox_NH3_GAS->setText(QCoreApplication::translate("MainWindow", "NH3", nullptr));
        checkBox_NO2_GAS->setText(QCoreApplication::translate("MainWindow", "NO2", nullptr));
        checkBox_Motor_GAS->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\346\216\247\345\210\266", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "MAC", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "\344\270\212\344\274\240\345\221\250\346\234\237", nullptr));
        box_sensor_u4->setItemText(0, QCoreApplication::translate("MainWindow", "NONE", nullptr));
        box_sensor_u4->setItemText(1, QCoreApplication::translate("MainWindow", "CH4", nullptr));
        box_sensor_u4->setItemText(2, QCoreApplication::translate("MainWindow", "NH3", nullptr));
        box_sensor_u4->setItemText(3, QCoreApplication::translate("MainWindow", "CO", nullptr));
        box_sensor_u4->setItemText(4, QCoreApplication::translate("MainWindow", "SO2", nullptr));
        box_sensor_u4->setItemText(5, QCoreApplication::translate("MainWindow", "H2S", nullptr));
        box_sensor_u4->setItemText(6, QCoreApplication::translate("MainWindow", "NO2", nullptr));
        box_sensor_u4->setItemText(7, QCoreApplication::translate("MainWindow", "CO2", nullptr));
        box_sensor_u4->setItemText(10, QCoreApplication::translate("MainWindow", "PM25", nullptr));

        label_20->setText(QCoreApplication::translate("MainWindow", "UART4", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "UART5", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "UART6", nullptr));
        pushButton_typeset_Gas->setText(QCoreApplication::translate("MainWindow", "\347\261\273\345\236\213\350\256\276\345\256\232", nullptr));
        box_sensor_u5->setItemText(0, QCoreApplication::translate("MainWindow", "NONE", nullptr));
        box_sensor_u5->setItemText(1, QCoreApplication::translate("MainWindow", "CH4", nullptr));
        box_sensor_u5->setItemText(2, QCoreApplication::translate("MainWindow", "NH3", nullptr));
        box_sensor_u5->setItemText(3, QCoreApplication::translate("MainWindow", "CO", nullptr));
        box_sensor_u5->setItemText(4, QCoreApplication::translate("MainWindow", "SO2", nullptr));
        box_sensor_u5->setItemText(5, QCoreApplication::translate("MainWindow", "H2S", nullptr));
        box_sensor_u5->setItemText(6, QCoreApplication::translate("MainWindow", "NO2", nullptr));
        box_sensor_u5->setItemText(7, QCoreApplication::translate("MainWindow", "CO2", nullptr));
        box_sensor_u5->setItemText(10, QCoreApplication::translate("MainWindow", "PM25", nullptr));

        label_23->setText(QCoreApplication::translate("MainWindow", "\347\224\265\346\234\272\346\227\213\350\275\254", nullptr));
        pushButton_motor->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\350\275\254\345\212\250", nullptr));
        pushButton_periodset_Gas->setText(QCoreApplication::translate("MainWindow", "\345\221\250\346\234\237\350\256\276\345\256\232", nullptr));
        box_sensor_u6->setItemText(0, QCoreApplication::translate("MainWindow", "NONE", nullptr));
        box_sensor_u6->setItemText(1, QCoreApplication::translate("MainWindow", "CH4", nullptr));
        box_sensor_u6->setItemText(2, QCoreApplication::translate("MainWindow", "NH3", nullptr));
        box_sensor_u6->setItemText(3, QCoreApplication::translate("MainWindow", "CO", nullptr));
        box_sensor_u6->setItemText(4, QCoreApplication::translate("MainWindow", "SO2", nullptr));
        box_sensor_u6->setItemText(5, QCoreApplication::translate("MainWindow", "H2S", nullptr));
        box_sensor_u6->setItemText(6, QCoreApplication::translate("MainWindow", "NO2", nullptr));
        box_sensor_u6->setItemText(7, QCoreApplication::translate("MainWindow", "CO2", nullptr));
        box_sensor_u6->setItemText(10, QCoreApplication::translate("MainWindow", "PM25", nullptr));

        tabWidget->setTabText(tabWidget->indexOf(GAS), QCoreApplication::translate("MainWindow", "\346\260\224\344\275\223", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "TX Data", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        pushButton_txclear->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\345\217\221\351\200\201", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\225\260\346\215\256\346\240\274\345\274\217", nullptr));
        txform_ASCII->setText(QCoreApplication::translate("MainWindow", "ASCII", nullptr));
        txform_HEX->setText(QCoreApplication::translate("MainWindow", "HEX", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
