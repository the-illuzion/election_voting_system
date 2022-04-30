import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import {FormControl, FormGroupDirective, NgForm, Validators} from '@angular/forms';
import {ErrorStateMatcher} from '@angular/material/core';
import { Router } from '@angular/router';
import { VotingService } from '../../service/voting.service';
import { AlertService } from '../../service/alert.service';

export class MyErrorStateMatcher implements ErrorStateMatcher {
  isErrorState(control: FormControl | null, form: FormGroupDirective | NgForm | null): boolean {
    const isSubmitted = form && form.submitted;
    return !!(control && control.invalid && (control.dirty || control.touched || isSubmitted));
  }
}

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {
  @ViewChild('email') email: ElementRef;
  @ViewChild('password') password: ElementRef;
  
  hide = true;
  constructor(public votingservice : VotingService, private router : Router, emailElement: ElementRef, passwordElement: ElementRef, private alertService: AlertService) { 
    this.email = emailElement;
    this.password = passwordElement;
  }

  ngOnInit(): void {
  }

  login(){
    let body = {
      email : this.email.nativeElement.value,
      password : this.password.nativeElement.value
    }
    this.votingservice.login(body).then((s: any) => {
      if(s["status"] == 200){
        this.router.navigate(['/main']);
      }else{
        this.alertService.error("Wrong Username/Password");
      }
    })
  }

}
