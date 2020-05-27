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
    ui->BackendErrorLabel->setText("");
    const size_t min_size = Parsing::ParseMin(ui->MineSizeBox->text());
    const size_t max_size = Parsing::ParseMax(ui->MaxSizeBox->text());
    const std::vector<ValidTypes> must_contain = Parsing::ParseMustContains(ui->ContainingBox->text());
    const std::vector<Substitution> substitutions = Parsing::ParseSubstitutions(ui->SubstitutionsBox->text());
    const pid_t pid = Parsing::GetPID(ui->PIDBox->text());
    if (pid==0){
        ui->BackendErrorLabel->setText("PID parsing failed. PID parsing return 0.");
        return;
    }

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
        0, //Min child
        0, //maxchild
        0, //min dec
        0, //max dec
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
        const size_t error_code = std::get<ANALYSE_PROGRAM_ERROR>(result);
        const auto error_string = ANALYSE_PROGRAM_ERROR_STR[error_code];
        std::cout << "backend return error value " << error_string << std::endl;
        ui->BackendErrorLabel->setText(error_string);
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

void MainWindow::on_SubstituteButton_clicked()
{
    std::cout << "Substituing now" << std::endl;
    ui->BackendErrorLabel->setText("");

    const pid_t pid = Parsing::GetPID(ui->PIDBox->text());
    if (pid==0){
        ui->BackendErrorLabel->setText("PID parsing failed. PID parsing return 0.");
        return;
    }
    std::cout << "pid:" << pid << std::endl;
    const std::vector<Substitution> substitutions = Parsing::ParseSubstitutions(ui->SubstitutionsBox->text());
    std::cout << "sub:" << std::get<int>(substitutions[0].from) << " :" << std::get<int>(substitutions[0].to) << std::endl;


    size_t* address =  Parsing::ParseSubstituteObjAddr(ui->SubstitutionsObjAddrBox->text());

    if (address == nullptr){
        ui->BackendErrorLabel->setText("Failed to parse object address");
        return;
    }
    std::cout << "addr:" << address << std::endl;

    const RemoteHeapPointer obj = {
        nullptr, //Address
        address, //Points to
        GetMallocMetadata(address,pid,819200,false,false),
        0,
        {}
    };
    size_t expected_write = 0;
          for (const auto& i : substitutions) {
             expected_write += std::visit(ValidTypesVisitor{}, i.from);
           }
           size_t bytes_written = RemoteMemory::Substitute(pid, obj, substitutions);
           if (expected_write != bytes_written) {
             std::cerr << "Write mismatch. Expected " << expected_write << " but only wrote " << bytes_written
                  << "\n";
           } else {
             std::cout << "Substitutions written out successfully to ptr at " << obj.actual_address
                  << ":" << obj.points_to << "\n";
          }
}
