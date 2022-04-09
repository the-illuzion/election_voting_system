import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { LoginComponent } from './Components/login/login.component';
import { MainComponent } from './Components/main/main.component';
import { CastVoteComponent } from './Components/cast-vote/cast-vote.component';

const routes: Routes = [
  { path : "", component : LoginComponent },
  { path : "main", component: MainComponent },
  { path : "cast-vote", component: CastVoteComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
