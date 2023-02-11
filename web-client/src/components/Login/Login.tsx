import React, { FormEvent } from 'react';
import { useNavigate } from 'react-router-dom';
import { PlayerService } from '../../services/PlayerService';
import { Button } from '../Button/Button';
import { FlexContainer } from '../FlexContainer/FlexContainer';

function Login() {
    const navigate = useNavigate();

    const onSubmit = (e: FormEvent<HTMLFormElement>) => {
        e.preventDefault();
        console.log(e.currentTarget.playerName.value);
        const playerName = e.currentTarget.playerName.value;
        const pwd = e.currentTarget.password.value;

        PlayerService.login(playerName, pwd).then((success) => {
            if(success) {
                navigate('/dashboard');
            } else {
                alert("Invalid login");
            }
        }).catch((e) => alert(`Could not log in: ${e.message}`));
    }

    return (
        <FlexContainer direction="column" justify="space-around" gap="2rem">
            <h1>Login</h1>
            <form onSubmit={onSubmit}>
                <FlexContainer direction="column" gap="1rem">
                    <input name="playerName" placeholder="Player name" />
                    <input name="password" type="password" placeholder="Password" />
                    <Button type="submit">Login</Button>
                </FlexContainer>
            </form>
        </FlexContainer>
    );
}

export { Login };
