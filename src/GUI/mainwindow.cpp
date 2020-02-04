#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->SearchBssCheckBox->setChecked(true);
    ui->SearchStackCheckBox->setChecked(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GoButton_clicked()
{
    const size_t min_size = Parsing::ParseMin(ui->MineSizeBox->text());
    const size_t max_size = Parsing::ParseMax(ui->MaxSizeBox->text());
    const std::vector<ValidTypes> must_contain = Parsing::ParseMustContains(ui->ContainingBox->text());
    const std::vector<Substitution> substitutions = Parsing::ParseSubstitutions(ui->SubstitutionsBox->text());
    const pid_t pid = Parsing::GetPID(ui->PIDBox->text());

    const bool search_bss = ui->SearchBssCheckBox->isChecked();
    const bool search_stack = ui->SearchStackCheckBox->isChecked();

    const Query q = {
        pid,
        true, //search_bss
        true, //search_stack
        search_bss,
        search_stack,
        0, //stack_frames_to_search (0 =all)
        8192, //max_heap_obj_size
        min_size,
        max_size,
        must_contain,
        substitutions
    };

    const auto result = ExecuteQuery(q);
    ui->OutputBox->setText("");
    for (const auto& i : result){
        InsertPtrToTextbox(i);
    }
}

std::vector<RemoteHeapPointer> MainWindow::ExecuteQuery(const Query q){
    AnalysisResultOrErr result = AnalyseProgram(q);
    if (result.index()==0){
        const auto matches = std::get<std::vector<RemoteHeapPointer>>(result);
        std::cout << "backend returned " << matches.size() << " matches" << std::endl;
        return matches;
    }else{
        //TODO handle/report error
        size_t error_code = std::get<ANALYSE_PROGRAM_ERROR>(result);
        std::cout << "backend return error value " << error_code << std::endl;
        return {};
    }
}

void MainWindow::InsertPtrToTextbox(const RemoteHeapPointer& ptr){
    std::cout << "Inserting" << ptr.actual_address << std::endl;
    ui->OutputBox->setText(ui->OutputBox->toPlainText() +
                           "Pointer Location:" + QString::number((size_t)ptr.actual_address,16) + "Points to:" +
                           QString::number((size_t)ptr.points_to,16) +
                           " size:" + QString::number(ptr.size_pointed_to) +
                           " children:" + QString::number(ptr.total_sub_pointers)
                           + '\n');
}
